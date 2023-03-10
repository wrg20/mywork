<?php
/**
 * @file
 * Implementation of DrupalNodeMigration for Drupal 7 sources.
 *
 * TODO: convert taxonomy term date into term names on migration instead of tid
 * TODO: Create a migrate all news/events/profiles and a limited migrate last
 *       100 news items or find a way to migrate only updated nodes from shared
 /

/**
 * Handling specific to a Drupal 7 source for nodes.
 */
class DrupalNode7Migration extends DrupalNodeMigration {

  /**
   * Holds the fileMigration attached to this migration.
   *
   * @var string
   */
  protected $fileMigration;

  /**
   * @param array $arguments
   */
  public function __construct(array $arguments) {
    parent::__construct($arguments);

    // Remove fields no longer relevent in D7 (now part of the body field).
    unset($this->sourceFields['format']);
    unset($this->sourceFields['teaser']);

    $this->source = new MigrateSourceSQL($this->query(), $this->sourceFields, NULL,
      $this->sourceOptions);

    $this->addFieldMapping('language', 'language')
         ->defaultValue($this->defaultLanguage);

    if (field_info_instance('node', 'body', $this->destinationType)) {
      $this->addFieldMapping('body:summary', 'body:summary');
      $this->addFieldMapping('body:format', 'body:format')
           ->callbacks(array($this, 'mapFormat'));
    }
    else {
      $this->addUnmigratedSources(array('body:summary', 'body:format'));
    }

    if (module_exists('media')) {
      $this->addFieldMapping('body', 'body', FALSE)
           ->callbacks(array($this, 'remapMediaJson'));
    }

    $this->addSimpleMappings(array('tnid', 'translate'));
  }

  /**
   * Query for basic node fields from Drupal 7.
   *
   * @return QueryConditionInterface
   */
  protected function query() {
    $query = Database::getConnection('default', $this->sourceConnection)
             ->select('node', 'n')
             ->fields('n', array('nid', 'vid', 'language', 'title', 'uid',
               'status', 'created', 'changed', 'comment', 'promote', 'sticky',
               'tnid', 'translate'))
             ->condition('n.type', $this->sourceType)
    // Add the different category IDs here as conditions
    //
    //  3 - Agricultural and Biological Engineering
    //
    //
             ->condition('fdfc.field_category_tid', array('3'))
             ->orderBy($this->newOnly ? 'n.nid' : 'n.changed');
    // Join node_counter for Statistics support
    if (Database::getConnection('default', $this->sourceConnection)
        ->schema()->tableExists('node_counter')) {
      $query->leftJoin('node_counter', 'nc', 'n.nid=nc.nid');
      $query->addField('nc', 'daycount');
      $query->addField('nc', 'timestamp');
      $query->addField('nc', 'totalcount');
    }
    // Join the taxonomy term to only pull in content tagged with a specific category
    if (Database::getConnection('default', $this->sourceConnection)
        ->schema()->tableExists('field_data_field_category')) {
      $query->leftJoin('field_data_field_category', 'fdfc', 'n.nid=fdfc.entity_id');
      $query->addField('fdfc', 'field_category_tid');
    }
    // Join the username for reference later
    if (Database::getConnection('default', $this->sourceConnection)
        ->schema()->tableExists('users')) {
      $query->leftJoin('users', 'users', 'n.uid=users.uid');
      $query->addField('users', 'name');
    }

    return $query;
  }

  public function prepareRow($row) {
    if (parent::prepareRow($row) === FALSE) {
      return FALSE;
    }

    // The property 'tnid' cannot be handled via sourceMigration() method
    // because it might be 0 or the main node of translation set. We don't want
    // to create a stub for such cases.
    if (!empty($row->tnid)) {
      $destination_ids = $this->getMap()->lookupDestinationID(array($row->tnid));
      // There's no destination yet. Create a stub.
      if (empty($destination_ids)) {
        // Don't create stub for itself.
        if ($row->tnid != $row->nid) {
          // Check if 'tnid' is a node in the source set to prevent not
          // updatable stubs.
          $query = clone $this->query();
          $query->condition('n.nid', $row->tnid);
          $nid = $query->execute()->fetchField();
          unset($query);
          if ($nid) {
            if ($tnids = $this->createStub(NULL)) {
              // Save the mapping.
              $this->map->saveIDMapping((object) array('nid' => $row->tnid), $tnids, MigrateMap::STATUS_NEEDS_UPDATE, $this->defaultRollbackAction);
              $row->tnid = reset($tnids);
            }
          }
        }
        else {
          $row->tnid = 0;
          $row->_is_translation_source = TRUE;
        }
      }
      else {
        $row->tnid = $destination_ids['destid1'];
      }
    }
  }

  public function complete($node, stdClass $row) {
    if (empty($row->_is_translation_source)) {
      return;
    }

    db_update('node')
      ->fields(array('tnid' => $node->nid))
      ->condition('nid', $node->nid)
      ->execute();
  }

  /**
   * Rewrites the fids contained within media module text json.
   *
   * @param array $values
   *   Incoming text values
   *
   * @return array
   *   The values array.
   */
  protected function remapMediaJson($values) {
    if (!is_array($values)) {
      $values = array($values);
    }

    $media_regex = '#\[\[(.*?\"type":"media".*?)\]\]#';

    if (!isset($this->fileMigration)) {
      $this->fileMigration = $this->findFileMigration();
    }

    // By this point, if we don't know the file migration we have to bail.
    if (empty($this->fileMigration)) {
      return $values;
    }

    $map = $this->fileMigration->getMap();

    foreach ($values as &$value) {
      preg_match_all($media_regex, $value, $matches);

      if (empty($matches[1])) {
        continue;
      }

      foreach ($matches[1] as $idx => $match) {
        $media_item = json_decode($match, TRUE);

        if (empty($media_item)) {
          continue;
        }

        $fid = $media_item['fid'];

        $destination = $map->lookupDestinationID(array($fid));

        if (empty($destination['destid1'])) {
          continue;
        }

        $media_item['fid'] = $destination['destid1'];
        $replace = '[[' . json_encode($media_item) . ']]';

        $value = str_replace($matches[0][$idx], $replace, $value);
      }
    }
    unset($value);

    return $values;
  }

  /**
   * Attempts to find a file migration for this class.
   *
   * First it will check for an argument passed in. Then it will look at
   * the migration's dependencies to try and find something with a destination
   * which is an instanceof MigrateDestinationFile.
   *
   * Returns FALSE if it can't find one.
   *
   * @return bool|Migration
   *   The Migration, or boolean FALSE.
   */
  protected function findFileMigration() {
    if (isset($this->arguments['file_migration'])) {
      return Migration::getInstance($this->arguments['file_migration']);
    }

    if (isset($this->arguments['soft_dependencies'])) {
      foreach ($this->arguments['soft_dependencies'] as $dep) {
        $migration = Migration::getInstance($dep);

        if ($migration->getDestination() instanceof MigrateDestinationFile) {
          return $migration;
        }
      }
    }

    if (isset($this->arguments['dependencies'])) {
      foreach ($this->arguments['dependencies'] as $dep) {
        $migration = Migration::getInstance($dep);

        if ($migration->getDestination() instanceof MigrateDestinationFile) {
          return $migration;
        }
      }
    }

    return FALSE;
  }

}

class EventMigration extends DrupalNode7Migration {
  public function __construct(array $arguments) {
    // Add any other data we're pulling into the source row, before the parent
    // constructor.

    parent::__construct($arguments);
      $this->addFieldMapping('uid')
           ->defaultValue(1);
      $this->addFieldMapping('field_sharedevent_created_by', 'name');
      //$this->addFieldMapping('language', 'language');
      //$this->addFieldMapping('body:language')->defaultValue('en');
      $this->addFieldMapping('field_sharedevent_event_type', 'field_event_type');
      $this->addFieldMapping('field_sharedevent_date', 'field_date');
      $this->addFieldMapping('field_sharedevent_date:to', 'field_date:value2');
      $this->addFieldMapping('field_sharedevent_date:timezone')
        ->defaultValue('America/Chicago');
      //$this->addFieldMapping('field_sharedevent_date:timezone')
      //  ->defaultValue('America/Chicago');
      $this->addFieldMapping('field_sharedevent_date:rrule')
         ->defaultValue('0');
      $this->addFieldMapping('field_sharedevent_location', 'field_location');
      $this->addFieldMapping('field_sharedevent_featured_img', 'event_feature_img');
      $this->addFieldMapping('field_sharedevent_featuredimgalt', 'event_feature_img_alt');
      $this->addFieldMapping('field_sharedevent_featuredimgcap', 'event_feature_img_cap');
      $this->addFieldMapping('field_sharedevent_other_imgs', 'event_other_imgs');
      $this->addFieldMapping('field_sharedevent_category', 'field_category');
      //$this->addFieldMapping('field_sharedevent_category:source_type')
      //  ->defaultValue('tid');
      //$this->addFieldMapping('field_sharedevent_category:create_term')
        //->defaultValue(TRUE);
      //$this->addFieldMapping('field_sharedevent_category:ignore_case')
      //  ->defaultValue(TRUE);
      $this->addFieldMapping('path', 'legacy_path');
  }

  function prepareRow($row) {
    if (parent::prepareRow($row) === FALSE) {
      return FALSE;
    }

    // this query is to pull the data for the associated event type "termreference" field and format it
    // into a display string. VID = 5
    $typequery = Database::getConnection('shared_migrate', $this->sourceConnection)
      ->select('taxonomy_term_data', 'term')
      ->fields('term', array('tid', 'name'))
      ->condition('tid', $row->field_event_type, '=');
    $type_row = $typequery->execute()->fetchObject();

    //transform code to create the display string
    if ($type_row) {
      $row->field_event_type = $type_row->name;
      //dpm($row->field_event_type_name);
    }

    // this query pulls the main feature image filename to use in creating an img tag back to the source image on shared
    // actual image files will not be copied over to the destination site
    $queryEvFeatureImg = Database::getConnection('shared_migrate', $this->sourceConnection)
      ->select('node', 'inode')
      ->fields('inode',array('nid'))
      ->condition('inode.nid', $row->nid, '=');
    $queryEvFeatureImg->leftJoin('field_data_field_featured_image', 'ftimg','ftimg.entity_id = inode.nid');
    $queryEvFeatureImg->leftJoin('file_managed','filemng', 'ftimg.field_featured_image_fid = filemng.fid');
    $queryEvFeatureImg->fields('ftimg', array('entity_id','field_featured_image_fid','field_featured_image_alt','field_featured_image_title'));
    $queryEvFeatureImg->fields('filemng', array('uri'));
    $Evfeatureimg_row = $queryEvFeatureImg->execute()->fetchObject();

    // transform code to create the img tag
    if ($Evfeatureimg_row->uri) {
      $row->event_feature_img = $Evfeatureimg_row->uri;
      if ($Evfeatureimg_row->field_featured_image_alt) {
        $row->event_feature_img_alt = $Evfeatureimg_row->field_featured_image_alt;
      }
      if ($Evfeatureimg_row->field_featured_image_title) {
        $row->event_feature_img_cap = $Evfeatureimg_row->field_featured_image_title;
      }
    }

    // this query pulls any additional images an article might have, and creates img tags similar to the feature img
    // since there may be multiple images in this field, need to create a list of tags for the template to use in a slideshow
    $queryEvOtherImgs = Database::getConnection('shared_migrate', $this->sourceConnection)
      ->select('node', 'imgnode')
      ->fields('imgnode',array('nid'))
      ->condition('imgnode.nid', $row->nid, '=');
    $queryEvOtherImgs->leftJoin('field_data_field_images','imgs','imgs.entity_id = imgnode.nid');
    $queryEvOtherImgs->leftJoin('file_managed','filesmng', 'imgs.field_images_fid = filesmng.fid');
    $queryEvOtherImgs->fields('imgs', array('entity_id','field_images_fid','field_images_alt','field_images_title','field_images_width','field_images_height'));
    $queryEvOtherImgs->fields('filesmng', array('filename'));
    $Evotherimgs_row = $queryEvOtherImgs->execute()->fetchObject();

    // transform code to create the img tag list
    $row->event_other_imgs = array();
    foreach ($Evotherimgs_row as $img_row) {
      if ($img_row->filename) {
        $imgLink = "<img src='http://shared.aces.illinois.edu/sites/shared.aces.illinois.edu/files/event/" . $img_row->filename . "'";
        if ($img_row->field_images_alt) {
          $imgLink .= " alt='" . $img_row->field_images_alt . "'";
        }
        if ($img_row->field_images_title) {
          $imgLink .= " title='" . $img_row->field_images_title . "'";
        }
        if ($img_row->field_images_width) {
          $imgLink .= " width='" . $img_row->field_images_width . "'";
        }
        if ($img_row->field_images_height) {
          $imgLink .= " height='" . $img_row->field_images_height . "'";
        }
        $imgLink .= " />"; //end image tag
        $row->event_other_imgs[] = $imgLink;
      }
    }
    $src_path = 'node/' . $row->nid;
    $node_path = Database::getConnection('shared_migrate', $this->sourceConnection)
      ->select('url_alias', 'ua')
      ->fields('ua', array('source', 'alias'))
      ->condition('source', $src_path, '=');
    $path_alias_row = $node_path->execute()->fetchObject();
    $explode_alias = explode('/', $path_alias_row->alias);
    $return_alias = $explode_alias[1];

    $row->legacy_path = 'calendar/' . $return_alias;
  }
}
class NewsMigration extends DrupalNode7Migration {
  public function __construct(array $arguments) {
    // Add any other data we're pulling into the source row, before the parent
    // constructor.

    parent::__construct($arguments);

    /* User ID has been set to administrator */
    $this->addFieldMapping('uid')
      ->defaultValue(1);
    $this->addFieldMapping('field_sharednews_author', 'newsAuthorValue');
    $this->addFieldMapping('field_sharednews_sources', 'newsSourceValue');
    $this->addFieldMapping('field_sharednews_featured_img', 'newsFeatureImg');
    $this->addFieldMapping('field_sharednews_featured_imgalt', 'field_featured_image_alt');
    $this->addFieldMapping('field_sharednews_featured_title', 'field_featured_image_title');
    $this->addFieldMapping('field_sharednews_other_imgs', 'imgsArray');
    $this->addFieldMapping('field_sharednews_related_links', 'field_related_link');
    $this->addFieldMapping('field_sharednews_photo_credit', 'field_photo_contact');
    $this->addFieldMapping('field_sharednews_media_link', 'field_media');
    $this->addFieldMapping('field_sharednews_source', 'field_source');
    $this->addFieldMapping('field_sharednews_category', 'field_category');
    $this->addFieldMapping('field_sharednews_newscategory', 'field_newscategory');
    $this->addFieldMapping('field_sharednews_category_other', 'field_newscategory_other');
    $this->addFieldMapping('field_sharednews_kill_date', 'field_killdate');
    $this->addFieldMapping('body:format')
      ->defaultValue('full_html');
    $this->addFieldMapping('path', 'legacy_path');
  }

  function prepareRow($row) {
    if (parent::prepareRow($row) === FALSE) {
      return FALSE;
    }
    // this query is to pull the data for the associated author "nodereference" field and format it into a display
    // string with mailto link around author name if email provided, phone number if provided

    if (isset($row->field_author)) {
      $authquery = Database::getConnection('shared_migrate', $this->sourceConnection)
        ->select('node', 'authnode')
        ->fields('authnode',array('nid', 'title'))
        ->condition('authnode.nid', $row->field_author, '=');
      $authquery->leftJoin('field_data_field_news_author_email', 'authemail', 'authnode.nid = authemail.entity_id');
      $authquery->leftJoin('field_data_field_news_author_phone', 'authph', 'authnode.nid = authph.entity_id');
      $authquery->fields('authemail', array('field_news_author_email_value'));
      $authquery->fields('authph', array('field_news_author_phone_value'));
      $author_row = $authquery->execute()->fetchObject();

      if ($author_row) {
        $row->newsAuthorValue = $author_row->title;

        if ($author_row->field_news_author_email_value) {
           $testValue2 = "<a href='mailto:" . $author_row->field_news_author_email_value . "'>" . $author_row->title . "</a>";
           $row->newsAuthorValue = $testValue2;
        }
        if ($author_row->field_news_author_phone_value) {
          if ($testValue2) {
            $testValue3 = $testValue2 . ", " . $author_row->field_news_author_phone_value;
          }
          else {
            $testValue3 = $author_row->title . ", " . $author_row->field_news_author_phone_value;
          }
          $row->newsAuthorValue = $testValue3;
        }
      }
      else {
        $row->newsAuthorValue = '';
      }
    }
    else {
      $row->newsAuthorValue = "";
    }

    // this query is to pull the data for the associated Source(s) for an article and format it into a display string
    // mailto link around source name if email provided, phone numnber if provided
    $querySources = Database::getConnection('shared_migrate', $this->sourceConnection)
       ->select('field_data_field_sources', 'fs')
       ->fields('fs', array('field_sources_value'))
       ->condition('fs.entity_id', $row->nid, '=');
    $querySources->leftJoin('field_data_field_sources_name','fn', 'fs.field_sources_value = fn.entity_id');
    $querySources->leftJoin('field_data_field_sources_email','fe', 'fs.field_sources_value = fe.entity_id');
    $querySources->leftJoin('field_data_field_sources_phone','fp', 'fs.field_sources_value = fp.entity_id');
    $querySources->fields('fn', array('field_sources_name_value'));
    $querySources->fields('fe', array('field_sources_email_value'));
    $querySources->fields('fp', array('field_sources_phone_value'));
    $sources_row = $querySources->execute()->fetchObject();

    // transform code to create the display string
    if ($sources_row) {
      $row->newsSourceValue = "<div class='newsSources'>";

      if ($sources_row->field_sources_email_value) {
        $row->newsSourceValue = $row->newsSourceValue . "<a href='mailto:" . $sources_row->field_sources_email_value . "'>" .
        $sources_row->field_sources_name_value . "</a>";
      }
      else {
        $row->newsSourceValue = $row->newsSourceValue . $sources_row->field_sources_name_value;
      }
      if ($sources_row->field_sources_phone_value) {
        $row->newsSourceValue = $row->newsSourceValue . ", " .
        $sources_row->field_sources_phone_value;
      }
      $row->newsSourceValue = $row->newsSourceValue . "</div>";
    }
    else {
      $row->newsSourceValue = 'No sources found';
    }

    // this query pulls the main feature image filename to use in creating an img tag back to the source image
    // actual image files will not be copied over to the destination site
    $queryFeatureImg = Database::getConnection('shared_migrate', $this->sourceConnection)
      ->select('node', 'inode')
      ->fields('inode',array('nid'))
      ->condition('inode.nid', $row->nid, '=');
    $queryFeatureImg->leftJoin('field_data_field_featured_image', 'ftimg','ftimg.entity_id = inode.nid');
    $queryFeatureImg->leftJoin('file_managed','filemng', 'ftimg.field_featured_image_fid = filemng.fid');
    $queryFeatureImg->fields('ftimg', array('entity_id','field_featured_image_fid','field_featured_image_alt','field_featured_image_title','field_featured_image_width','field_featured_image_height'));
    $queryFeatureImg->fields('filemng', array('filename', 'uri'));
    $featureimg_row = $queryFeatureImg->execute()->fetchObject();


     //pull out the fields and assign them variables that can be mapped
    if ($featureimg_row->uri) {
      $featureimg_uri = str_replace("public://news/","",$featureimg_row->uri);
      $row->newsFeatureImg = $featureimg_uri ;
    }

    // transform code to create the img tag
    if ($featureimg_row->filename) {

      if ($featureimg_row->field_featured_image_alt) {
        $row->field_featured_image_alt = $featureimg_row->field_featured_image_alt;
      }
      if ($featureimg_row->field_featured_image_title) {
        $row->field_featured_image_title = $featureimg_row->field_featured_image_title;
      }
      // if ($featureimg_row->field_featured_image_width) {
      //   $row->newsFeatureImg .= " width='" . $featureimg_row->field_featured_image_width . "'";
      // }
      // if ($featureimg_row->field_featured_image_height) {
      //   $row->newsFeatureImg .= " height='" . $featureimg_row->field_featured_image_height . "'";
      // }
    }

    // this query pulls any additional images an article might have, and creates img tags similar to the feature img
    // as there may be multiple images, need to create a list of tags for the template to use in a slideshow

    $row->imgsArray = array();

    $queryOtherImgs = Database::getConnection('shared_migrate', $this->sourceConnection)
      ->select('node', 'imgnode')
      ->fields('imgnode',array('nid'))
      ->condition('imgnode.nid', $row->nid, '=');
    $queryOtherImgs->leftJoin('field_data_field_images','imgs','imgs.entity_id = imgnode.nid');
    $queryOtherImgs->leftJoin('file_managed','filesmng','imgs.field_images_fid = filesmng.fid');
    $queryOtherImgs->fields('imgs', array('entity_id','field_images_fid','field_images_alt','field_images_title','field_images_width','field_images_height'));
    $queryOtherImgs->fields('filesmng', array('filename', 'uri'));
    $otherimgs_row = $queryOtherImgs->execute();

         // transform code to create the img tag list

    foreach ($otherimgs_row as $img_row) {

      if ($img_row->filename) {
        //$filename = $img_row->filename;
        $image_alt = $img_row->field_images_alt;
        $image_title = $img_row->field_images_title;
        $image_width = $img_row->field_images_width;
        $image_height = $img_row->field_images_height;
        $imgLink = "<img src='http://shared.aces.illinois.edu/sites/shared.aces.illinois.edu/files/news/" . $img_row->filename ."'";
        $imgLink .= " title='" . $image_title . "'";
        $imgLink .= " alt='" . $image_alt . "'";
        $imgLink .= " width='" . $image_width . "'";
        $imgLink .= " height='" . $image_height . "'";
        $imgLink .= " />";
        $row->imgsArray[] = $imgLink;
      }
      else {
        $row->imgsArray[] = "";
      }
    }






    //     $imgLink = "<img src='http://shared.aces.illinois.edu/sites/shared.aces.illinois.edu/files/news/" . $img_row->filename . "'";
    //     if ($img_row->field_images_alt) {
    //       $imgLink .= " alt='" . $img_row->field_images_alt . "'";
    //     }
    //     if ($img_row->field_images_title) {
    //       $imgLink .= " title='" . $img_row->field_images_title . "'";
    //     }
    //     if ($img_row->field_images_width) {
    //       $imgLink .= " width='" . $img_row->field_images_width . "'";
    //     }
    //     if ($img_row->field_images_height) {
    //       $imgLink .= " height='" . $img_row->field_images_height . "'";
    //     }
    //     $imgLink .= " />"; //end image tag
    //     $row->imgsArray[] = $imgLink;
    //   }
    // }

    // Change the url alias from /content to /news
    $src_path = 'node/' . $row->nid;
    $node_path = Database::getConnection('shared_migrate', $this->sourceConnection)
      ->select('url_alias', 'ua')
      ->fields('ua', array('source', 'alias'))
      ->condition('source', $src_path, '=');
    $path_alias_row = $node_path->execute()->fetchObject();
    $explode_alias = explode('/', $path_alias_row->alias);
    $return_alias = $explode_alias[1];

    $row->legacy_path = 'news/' . $return_alias;
  }
}
class ProfileMigration extends DrupalNode7Migration {
  public function __construct(array $arguments) {
    // Add any other data we're pulling into the source row, before the parent
    // constructor.

    parent::__construct($arguments);

    /* User ID has been set to administrator */
    $this->addFieldMapping('uid')
      ->defaultValue(1);
    $this->addFieldMapping('field_sharedprofile_type', 'field_type');
    $this->addFieldMapping('field_sharedprofile_kill_date', 'field_expiration_date');
    $this->addFieldMapping('field_sharedprofile_thumbnail', 'field_thumbnail');
    $this->addFieldMapping('field_sharedprofile_thumb_alt', 'field_thumbnail:alt');
    $this->addFieldMapping('field_sharedprofile_thumb_cap', 'field_thumbnail:title');
    $this->addFieldMapping('field_sharedprofile_image', 'field_image');
    $this->addFieldMapping('field_sharedprofile_img_alt', 'field_image:alt');
    $this->addFieldMapping('field_sharedprofile_img_cap', 'field_image:title');
    $this->addFieldMapping('field_sharedprofile_short_career', 'field_career');
    $this->addFieldMapping('field_sharedprofile_career_desc', 'field_career_description');
    $this->addFieldMapping('field_sharedprofile_career_desc:format')
      ->defaultValue('full_html');
    $this->addFieldMapping('field_sharedprofile_department', 'field_title');
    $this->addFieldMapping('field_sharedprofile_major', 'field_concentration');
    $this->addFieldMapping('field_sharedprofile_hometown', 'field_hometown');
    $this->addFieldMapping('field_sharedprofile_quote', 'field_quote');
    $this->addFieldMapping('field_sharedprofile_category', 'field_category');
    //$this->addFieldMapping('field_sharedprofile_category:source_type');
       // ->defaultValue('tid');
    $this->addFieldMapping('body:format')
      ->defaultValue('full_html');
    $this->addFieldMapping('path', 'legacy_path');
  }

  function prepareRow($row) {
    if (parent::prepareRow($row) === FALSE) {
      return FALSE;
    }
    // this query is to pull the data for the associated event type "termreference" field and format it
    // into a display string. VID = 5
    $typequery = Database::getConnection('shared_migrate', $this->sourceConnection)
      ->select('taxonomy_term_data', 'term')
      ->fields('term', array('tid', 'name'))
      ->condition('tid', $row->field_type, '=');
    $type_row = $typequery->execute()->fetchObject();

    //pull out the fields and assign them variables that can be mapped
    if ($type_row) {
      $row->field_type = $type_row->name;
    }

    // this query pulls the main feature image filename to use in creating an img tag back to the source image
    // actual image files will not be copied over to the destination site
    $queryProfileImg = Database::getConnection('shared_migrate', $this->sourceConnection)
      ->select('node', 'inode')
      ->fields('inode',array('nid'))
      ->condition('inode.nid', $row->nid, '=');
    $queryProfileImg->leftJoin('field_data_field_image', 'fdfi','fdfi.entity_id = inode.nid');
    $queryProfileImg->leftJoin('file_managed','filemng', 'fdfi.field_image_fid = filemng.fid');
    $queryProfileImg->fields('fdfi', array('entity_id','field_image_fid','field_image_alt','field_image_title'));
    $queryProfileImg->fields('filemng', array('uri'));
    $profileimg_row = $queryProfileImg->execute()->fetchObject();

    //pull out the fields and assign them variables that can be mapped
    if ($profileimg_row->uri) {
      $profile_image_uri = str_replace("public://profile/","",$profileimg_row->uri);
      $row->field_image = $profile_image_uri ;
    }

    $queryProfileThumbnail = Database::getConnection('shared_migrate', $this->sourceConnection)
      ->select('node', 'inode')
      ->fields('inode',array('nid'))
      ->condition('inode.nid', $row->nid, '=');
    $queryProfileThumbnail->leftJoin('field_data_field_thumbnail', 'fdft', 'fdft.entity_id = inode.nid');
    $queryProfileThumbnail->leftJoin('file_managed','filemng', 'fdft.field_thumbnail_fid = filemng.fid');
    $queryProfileThumbnail->fields('fdft', array('entity_id', 'field_thumbnail_fid', 'field_thumbnail_alt', 'field_thumbnail_title'));
    $queryProfileThumbnail->fields('filemng', array('uri'));
    $profilethumb_row = $queryProfileThumbnail->execute()->fetchObject();

    //pull out the fields and assign them variables that can be mapped
    if ($profilethumb_row->uri) {
      $profile_thumbnail_uri = str_replace("public://profile/","",$profilethumb_row->uri);
      $row->field_thumbnail = $profile_thumbnail_uri;
    }
    $src_path = 'node/' . $row->nid;
    $node_path = Database::getConnection('shared_migrate', $this->sourceConnection)
      ->select('url_alias', 'ua')
      ->fields('ua', array('source', 'alias'))
      ->condition('source', $src_path, '=');
    $path_alias_row = $node_path->execute()->fetchObject();
    $explode_alias = explode('/', $path_alias_row->alias);
    $return_alias = $explode_alias[1];

    $row->legacy_path = 'profile/' . $return_alias;


    //only pull profiles will expiration dates in the future
    /*$expirationQuery = Database::getConnection('shared_migrate', $this->sourceConnection)
      ->select('node', 'inode')
      ->fields('inode',array('nid'))
      ->condition('inode.nid', $row->nid, '=');
    $expirationQuery->leftJoin('field_data_field_expiration_date', 'fdfed', 'fdfed.entity_id = inode.nid');
    $expirationQuery->fields('fdfed', array('entity_id', 'field_expiration_date_value'));
    $expiration_row = $expirationQuery->execute()->fetchObject();

    //pull out the fields and assign them variables that can be mapped
    $today = date("Y-m-d\TH:i:s");
    $dataDate = date("Y-m-d\TH:i:s", strtotime($expiration_row->field_expiration_date_value));*/

    /*if ($expiration_row) {
      if ($dataDate < $today) {
        return FALSE;
      }
      else {
        $row->field_expiration_date = $expiration_row->field_expiration_date_value;
        return TRUE;
      }
    }*/
  }
}
class ScholarshipMigration extends DrupalNode7Migration {
  public function __construct(array $arguments) {
    // Add any other data we're pulling into the source row, before the parent
    // constructor.

    parent::__construct($arguments);

    /* User ID has been set to administrator */
    $this->addFieldMapping('uid')
      ->defaultValue(1);
    $this->addFieldMapping('field_sharedscholar_sort_name', 'field_sort_name');
    $this->addFieldMapping('field_sharedscholar_category', 'field_category');
    $this->addFieldMapping('field_sharedscholar_type', 'field_scholarship_type');
    $this->addFieldMapping('field_sharedscholar_schocategory', 'field_scholarship_category');
    $this->addFieldMapping('field_sharedscholar_yearly_amt', 'field_amount');
    $this->addFieldMapping('field_sharedscholar_deadline', 'field_deadline');
    $this->addFieldMapping('field_sharedscholar_need', 'field_financial_need');
    $this->addFieldMapping('body:format')
      ->defaultValue('full_html');
    $this->addFieldMapping('path', 'legacy_path');
  }

  function prepareRow($row) {
    if (parent::prepareRow($row) === FALSE) {
      return FALSE;
    }
    $src_path = 'node/' . $row->nid;
    $node_path = Database::getConnection('shared_migrate', $this->sourceConnection)
      ->select('url_alias', 'ua')
      ->fields('ua', array('source', 'alias'))
      ->condition('source', $src_path, '=');
    $path_alias_row = $node_path->execute()->fetchObject();
    $explode_alias = explode('/', $path_alias_row->alias);
    $return_alias = $explode_alias[1];
    $row->legacy_path = 'scholarship/' . $return_alias;
  }
}
class ConcentrationMigration extends DrupalNode7Migration {
  public function __construct(array $arguments) {
    // Add any other data we're pulling into the source row, before the parent
    // constructor.

    parent::__construct($arguments);

    /* User ID has been set to administrator */
    $this->addFieldMapping('uid')
      ->defaultValue(1);
    $this->addFieldMapping('field_sharedconcen_category', 'field_category');
    $this->addFieldMapping('field_sharedconcen_overview', 'field_overview');
    $this->addFieldMapping('field_sharedconcen_highlight', 'field_highlight');
    $this->addFieldMapping('field_sharedconcen_related_links', 'field_related_links');
    $this->addFieldMapping('field_sharedconcen_featured', 'field_profile');
    $this->addFieldMapping('body:format')
      ->defaultValue('full_html');
    $this->addFieldMapping('path', 'legacy_path');
  }

  function prepareRow($row) {
    if (parent::prepareRow($row) === FALSE) {
      return FALSE;
    }
    $src_path = 'node/' . $row->nid;
    $node_path = Database::getConnection('shared_migrate', $this->sourceConnection)
      ->select('url_alias', 'ua')
      ->fields('ua', array('source', 'alias'))
      ->condition('source', $src_path, '=');
    $path_alias_row = $node_path->execute()->fetchObject();
    $explode_alias = explode('/', $path_alias_row->alias);
    $return_alias = $explode_alias[1];
    $row->legacy_path = 'concentration/' . $return_alias;
  }
}
