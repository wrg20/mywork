<?php

/**
 * Common mappings for the Drupal 7 node migrations.
 */
abstract class SharedNodeMigration extends DrupalNode7Migration {
  public function __construct(array $arguments) {
    parent::__construct($arguments);

  }
}

/**
 * Image-specific mappings and handling.
 */

class SharedImageMigration extends SharedNodeMigration {
  public function __construct(array $arguments) {
    // Add any other data we're pulling into the source row, before the parent
    // constructor.
    //$this->sourceFields['authorNID'] = 'NID from the author node reference';
    $this->sourceFields['filename'] = 'Path of image within mounted filesystem';

    parent::__construct($arguments);

    $arguments = array('source_path' => '/mnt/source_images');
    $this->addFieldMapping('field_image', 'filename')
         ->arguments($arguments);
  }

  /**
   * Implementation of Migration::prepareRow().
   *
   * Here we pull in additional data from a non-core, non-CCK table.
   *
   * @param $row
   */
  public function prepareRow($row) {
    if (parent::prepareRow($row) === FALSE) {
      return FALSE;
    }

    $image_row = Database::getConnection('shared_migrate', $this->sourceConnection)
                 ->select('custom_image_table', 'i')
                 ->fields('i', array('copyright', 'filename'))
                 ->condition('vid', $row->vid)
                 ->execute()
                 ->fetchObject();
    if ($image_row) {
      $row->copyright = $image_row->copyright;
      $row->filename = $image_row->filename;
    }
    else {
      $this->saveMessage(t('No custom_image_table data for image node with vid !vid',
                         array('!vid' => $row->vid)));
    }


  }
}

class NewsArticleMigration extends SharedNodeMigration {
//limit our main query to only pull specific categories of articles. Also, if there's a time limit, add that condition

  public function __construct(array $arguments) {
    // Add any other data we're pulling into the source row, before the parent
    // constructor.

    parent::__construct($arguments);

/* User ID has been set to administrator */
      $this->addFieldMapping('uid')
           ->defaultValue(1);
      $this->addFieldMapping('field_author_text', 'newsAuthorValue');
      $this->addFieldMapping('field_killdate', 'field_killdate');
      $this->addFieldMapping('field_sources_text', 'newsSourceValue');
      $this->addFieldMapping('field_featuredimage', 'newsFeatureImg');
      $this->addFieldMapping('field_otherimages', 'imgsArray');
      $this->addFieldMapping('field_related_links', 'field_related_link');
      $this->addFieldMapping('body:format')
          ->defaultValue('full_html');
      
  }

  function prepareRow($row) {
    if (parent::prepareRow($row) === FALSE) {
      return FALSE;
    }

     //limit rows processed to only those rows which match the category for this site
/**********************************************************************************
/
/
/ !!!!!!!!!!!!!!!!!
/ !!!!!!!!!!!!!!!!! CHANGE THE NUMBER BELOW TO THE TERM ID OF THE 
/ !!!!!!!!!!!!!!!!! TAXONOMY "SHARED CATEGORY" ON THE SHARED.ACES.ILLINOIS.EDU SITE
/ !!!!!!!!!!!!!!!!! 
/
/
********************************************************************************** */
     $check_category = array(10,1486,1487,1488,1489);
     if (! isset($row->field_category) || ! in_array("10",$row->field_category) ) {
        return FALSE;
     } else {
        //dpm($row->field_category);
     }

     // this query is to pull the data for the associated author "nodereference" field and format it into a display
     // string with mailto link around author name if email provided, phone number if provided
     $authquery = Database::getConnection('shared_migrate', $this->sourceConnection)
                 ->select('node', 'authnode')
                 ->fields('authnode', array('nid', 'title'))
                 ->condition('authnode.nid', $row->field_author, '=');
     $authquery->leftJoin('field_data_field_news_author_email', 'authemail', 'authnode.nid = authemail.entity_id');
     $authquery->leftJoin('field_data_field_news_author_phone', 'authph', 'authnode.nid = authph.entity_id');     
     $authquery->fields('authemail', array('field_news_author_email_value'));
     $authquery->fields('authph', array('field_news_author_phone_value'));

     $author_row = $authquery->execute()->fetchObject();

//dpq($authquery);
//dpm($author_row);
    //transform code to create the display string

    if ($author_row) {
      $row->newsAuthorValue = $author_row->title;

      if ($author_row->field_news_author_email_value) {
         $testValue2 = "<a href='mailto:" . $author_row->field_news_author_email_value . "'>" . $author_row->title . "</a>";
         $row->newsAuthorValue = $testValue2;
      }
      if ($author_row->field_news_author_phone_value) {
         if ($testValue2) {
             $testValue3 = $testValue2 . ", " . $author_row->field_news_author_phone_value;
         } else {
             $testValue3 = $author_row->title . ", " . $author_row->field_news_author_phone_value;
         }
         $row->newsAuthorValue = $testValue3;
      }
    } else { $row->newsAuthorValue = ''; }

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
                $row->newsSourceValue = $row->newsSourceValue . "<a href='mailto:" . $sources_row->field_sources_email_value . "'>" . $sources_row->field_sources_name_value . "</a>";
            } else {
                $row->newsSourceValue = $row->newsSourceValue . $sources_row->field_sources_name_value;
            }

            if ($sources_row->field_sources_phone_value) {
                $row->newsSourceValue = $row->newsSourceValue . ", " . $sources_row->field_sources_phone_value;
            }

            $row->newsSourceValue = $row->newsSourceValue . "</div>";

    } else { $row->newsSourceValue = 'No sources found'; }

    // this query pulls the main feature image filename to use in creating an img tag back to the source image
    // actual image files will not be copied over to the destination site
    $queryFeatureImg = Database::getConnection('shared_migrate', $this->sourceConnection) 
        ->select('node', 'inode')
        ->fields('inode',array('nid'))
        ->condition('inode.nid', $row->nid, '='); 
    $queryFeatureImg->leftJoin('field_data_field_featured_image', 'ftimg','ftimg.entity_id = inode.nid');
    $queryFeatureImg->leftJoin('file_managed','filemng', 'ftimg.field_featured_image_fid = filemng.fid');
    $queryFeatureImg->fields('ftimg', array('entity_id','field_featured_image_fid','field_featured_image_alt','field_featured_image_title','field_featured_image_width','field_featured_image_height')); 
    $queryFeatureImg->fields('filemng', array('filename'));

     $featureimg_row = $queryFeatureImg->execute()->fetchObject();

    // transform code to create the img tag
    if ($featureimg_row->filename) {
           $row->newsFeatureImg = "<img src='http://shared.aces.illinois.edu/sites/shared.aces.illinois.edu/files/news/" . $featureimg_row->filename . "'"; 
        if ($featureimg_row->field_featured_image_alt) {
           $row->newsFeatureImg .= " alt='" . $featureimg_row->field_featured_image_alt . "'";
        }
        if ($featureimg_row->field_featured_image_title) {
           $row->newsFeatureImg .= " title='" . $featureimg_row->field_featured_image_title . "'";
        }
        if ($featureimg_row->field_featured_image_width) {
           $row->newsFeatureImg .= " width='" . $featureimg_row->field_featured_image_width . "'";
        }
        if ($featureimg_row->field_featured_image_height) {
           $row->newsFeatureImg .= " height='" . $featureimg_row->field_featured_image_height . "'";
        }
        $row->newsFeatureImg .= " />"; //end image tag
    }


    // this query pulls any additional images an article might have, and creates img tags similar to the feature img
    // as there may be multiple images, need to create a list of tags for the template to use in a slideshow
    $queryOtherImgs = Database::getConnection('shared_migrate', $this->sourceConnection)
        ->select('node', 'imgnode')
        ->fields('imgnode',array('nid'))
        ->condition('imgnode.nid', $row->nid, '=');
    $queryOtherImgs->leftJoin('field_data_field_images','imgs','imgs.entity_id = imgnode.nid');
    $queryOtherImgs->leftJoin('file_managed','filesmng', 'imgs.field_images_fid = filesmng.fid');
    $queryOtherImgs->fields('imgs', array('entity_id','field_images_fid','field_images_alt','field_images_title','field_images_width','field_images_height'));
    $queryOtherImgs->fields('filesmng', array('filename'));

     $otherimgs_row = $queryOtherImgs->execute()->fetchObject();


    // transform code to create the img tag list
    $row->imgsArray = array();
    foreach ($otherimgs_row as $img_row) {
        if ($img_row->filename) {
            $imgLink = "<img src='http://shared.aces.illinois.edu/sites/shared.aces.illinois.edu/files/news/" . $img_row->filename . "'";
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
            $row->imgsArray[] = $imgLink;
        }
    }

  }
  
}

class EventArticleMigration extends SharedNodeMigration {
  public function __construct(array $arguments) {
    // Add any other data we're pulling into the source row, before the parent
    // constructor.

    parent::__construct($arguments);

      $this->addFieldMapping('uid')
           ->defaultValue(1);
      $this->addFieldMapping('field_event_type_name', 'field_event_type');
      $this->addFieldMapping('field_date', 'field_date');
      $this->addFieldMapping('field_date:to', 'field_date:value2');
      $this->addFieldMapping('field_date:timezone')
         ->defaultValue('America/Chicago');
      $this->addFieldMapping('field_location', 'field_location');
      $this->addFieldMapping('field_ev_featureimage', 'newsFeatureImg');
      $this->addFieldMapping('field_ev_otherimages', 'imgsArray');
     // $this->addFieldMapping('field_ev_category', 'field_category');

  }

  function prepareRow($row) {
    if (parent::prepareRow($row) === FALSE) {
      return FALSE;
    }

// ************************************************************************************
// *
// * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  IMPORTANT  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// *
// * Limit rows processed to only those rows which match the category for this site
// * CHANGE THE $check_category ARRAY BELOW TO MATCH THE CATEGORY FOR THIS SITE!!! 
// *
// * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  IMPORTANT  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// *
// ************************************************************************************
     $check_category = array("10","1486","1487","1488","1489");
     if (! isset($row->field_category) || ! in_array("10",$row->field_category) ) {
        return FALSE;
     } else {
       // dpm($row->field_category);
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

// this query pulls the main feature image filename to use in creating an img tag back to the source image
// actual image files will not be copied over to the destination site
    $queryEvFeatureImg = Database::getConnection('shared_migrate', $this->sourceConnection)
        ->select('node', 'inode')
        ->fields('inode',array('nid'))
        ->condition('inode.nid', $row->nid, '=');
    $queryEvFeatureImg->leftJoin('field_data_field_featured_image', 'ftimg','ftimg.entity_id = inode.nid');
    $queryEvFeatureImg->leftJoin('file_managed','filemng', 'ftimg.field_featured_image_fid = filemng.fid');
    $queryEvFeatureImg->fields('ftimg', array('entity_id','field_featured_image_fid','field_featured_image_alt','field_featured_image_title','field_featured_image_width','field_featured_image_height'));

    $queryEvFeatureImg->fields('filemng', array('filename'));

     $Evfeatureimg_row = $queryEvFeatureImg->execute()->fetchObject();

// transform code to create the img tag

    if ($Evfeatureimg_row->filename) {
           $row->newsFeatureImg = "<img src='http://shared.aces.illinois.edu/sites/shared.aces.illinois.edu/files/news/" . $Evfeatureimg_row->filename . "'";
        if ($Evfeatureimg_row->field_featured_image_alt) {
           $row->newsFeatureImg .= " alt='" . $Evfeatureimg_row->field_featured_image_alt . "'";
        }
        if ($Evfeatureimg_row->field_featured_image_title) {
           $row->newsFeatureImg .= " title='" . $Evfeatureimg_row->field_featured_image_title . "'";
        }
        if ($Evfeatureimg_row->field_featured_image_width) {
           $row->newsFeatureImg .= " width='" . $Evfeatureimg_row->field_featured_image_width . "'";
        }
        if ($Evfeatureimg_row->field_featured_image_height) {
           $row->newsFeatureImg .= " height='" . $Evfeatureimg_row->field_featured_image_height . "'";
        }
        $row->newsFeatureImg .= " />"; //end image tag
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
    $row->imgsArray = array();
    foreach ($Evotherimgs_row as $img_row) {
        if ($img_row->filename) {
            $imgLink = "<img src='http://shared.aces.illinois.edu/sites/shared.aces.illinois.edu/files/news/" . $img_row->filename . "'";
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
            $row->imgsArray[] = $imgLink;
        }
    }

  }

}