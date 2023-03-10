<?php
/**
 * @file migrate_d7d7_migrate.migrate.inc
 */
function migrate_d7d7_migrate_flush_caches() {
  migrate_d7d7_migrate_register_migrations();
}


function migrate_d7d7_migrate_register_migrations() {
  /*$common_arguments = array(
    'source_connection' => 'shared_migrate',
    'source_version' => 7,
  );*/
  /*$node_arguments = array(
    'News' => array(
      //'class_name' => 'NewsMigration',
      'class_name' => 'DrupalNode7Migration',
      'description' => t('Migration of shared news nodes from Drupal 7'),
      'machine_name' => 'shared_news',
      'source_type' => 'shared_news',
      'destination_type' => 'shared_news',
      'group_name' => 'shared_content',
    ),
    'Events' => array(
      //'class_name' => 'EventMigration',
      'class_name' => 'DrupalNode7Migration',
      'description' => t('Migration of shared event nodes from Drupal 7'),
      'machine_name' => 'shared_event',
      'source_type' => 'shared_event',
      'destination_type' => 'shared_event',
      'group_name' => 'shared_content',
    ),
    'Profiles' => array(
      //'class_name' => 'ProfileMigration',
      'class_name' => 'DrupalNode7Migration',
      'description' => t('Migration of shared profile nodes from Drupal 7'),
      'machine_name' => 'shared_profile',
      'source_type' => 'shared_profile',
      'destination_type' => 'shared_profile',
      'group_name' => 'shared_content',
    ),
  );
  foreach ($node_arguments as $arguments) {
    $arguments += $common_arguments;
    Migration::registerMigration($arguments['class_name'], $arguments['machine_name'],$arguments);

  }*/
 /* $vocabulary_arguments = array(
    'shared_category' => array(
      'class_name' => 'DrupalTerm7Migration',
      'description' => t('Migration of Shared Category Terms from Shared'),
      'machine_name' => 'shared_category',
      'source_vocabulary' => 'shared_category',
      'destination_vocabulary' => 'shared_category',
      'group_name' => 'shared_taxonomies',
    ),
    'shared_event_type' => array(
      'class_name' => 'DrupalTerm7Migration',
      'description' => t('Migration of Shared Event Type Terms from Shared'),
      'machine_name' => 'shared_event_type',
      'source_vocabulary' => 'shared_event_type',
      'destination_vocabulary' => 'shared_event_type',
      'group_name' => 'shared_taxonomies',
    ),
    'shared_faculty_oversight' => array(
      'class_name' => 'DrupalTerm7Migration',
      'description' => t('Migration of Shared Faculty Oversight Terms from Shared'),
      'machine_name' => 'shared_faculty_oversight',
      'source_vocabulary' => 'shared_faculty_oversight',
      'destination_vocabulary' => 'shared_faculty_oversight',
      'group_name' => 'shared_taxonomies',
    ),
    'shared_fund_category' => array(
      'class_name' => 'DrupalTerm7Migration',
      'description' => t('Migration of Shared Fund Category Terms from Shared'),
      'machine_name' => 'shared_fund_category',
      'source_vocabulary' => 'shared_fund_category',
      'destination_vocabulary' => 'shared_fund_category',
      'group_name' => 'shared_taxonomies',
    ),
    'shared_news_category' => array(
      'class_name' => 'DrupalTerm7Migration',
      'description' => t('Migration of Shared News Category Terms from Shared'),
      'machine_name' => 'shared_news_category',
      'source_vocabulary' => 'shared_news_category',
      'destination_vocabulary' => 'shared_news_category',
      'group_name' => 'shared_taxonomies',
    ),
    'shared_profile_type' => array(
      'class_name' => 'DrupalTerm7Migration',
      'description' => t('Migration of Shared Profile Type Terms from Shared'),
      'machine_name' => 'shared_profile_type',
      'source_vocabulary' => 'shared_profile_type',
      'destination_vocabulary' => 'shared_profile_type',
      'group_name' => 'shared_taxonomies',
    ),
    'shared_scholarship_category' => array(
      'class_name' => 'DrupalTerm7Migration',
      'description' => t('Migration of Shared Scholarship Category Terms from Shared'),
      'machine_name' => 'shared_scholarship_category',
      'source_vocabulary' => 'shared_scholarship_category',
      'destination_vocabulary' => 'shared_scholarship_category',
      'group_name' => 'shared_taxonomies',
    ),
    'tags' => array(
      'class_name' => 'DrupalTerm7Migration',
      'description' => t('Migration of Tags from Shared'),
      'machine_name' => 'tags',
      'source_vocabulary' => 'tags',
      'destination_vocabulary' => 'tags',
      'group_name' => 'shared_taxonomies',
    ),
  );
  foreach ($vocabulary_arguments as $arguments) {
    $arguments += $common_arguments;
    Migration::registerMigration($arguments['class_name'], $arguments['machine_name'],$arguments);
  }*/
}
/**
 * Implements hook_migrate_api().
 */
function migrate_d7d7_migrate_migrate_api() {
  $api = array(
    'api' => 2,
    'groups' => array(
      'shared_content' => array(
        'title' => t('Shared Content'),
      ),
      /*'shared_taxonomies' => array(
        'title' => t('Shared Taxonomies'),
      ),*/
      /*'shared_users' => array(
        'title' => t('Shared Users'),
      ),*/
    ),
    'migrations' => array(
      // 'User' => array(
      //   'class_name' => 'DrupalUser7Migration',
      //   'description' => t('Migration of shared users from Drupal 7'),
      //   'machine_name' => 'User',
      //   //'source_type' => 'shared_news',
      //   //'destination_type' => 'shared_news',
      //   'group_name' => 'shared_users',
      //   'source_version' => 7,
      //   'source_connection' => 'shared_migrate',
      // ),
      'News' => array(
        'class_name' => 'NewsMigration',
        'description' => t('Migration of shared news nodes from Drupal 7'),
        'machine_name' => 'shared_news',
        'source_type' => 'shared_news',
        'destination_type' => 'shared_news',
        'group_name' => 'shared_content',
        'source_version' => 7,
        'source_connection' => 'shared_migrate',
      ),
      'Events' => array(
        'class_name' => 'EventMigration',
        'description' => t('Migration of shared event nodes from Drupal 7'),
        'machine_name' => 'shared_event',
        'source_type' => 'shared_event',
        'destination_type' => 'shared_event',
        'group_name' => 'shared_content',
        'source_version' => 7,
        'source_connection' => 'shared_migrate',
        //'soft_dependencies' => array('User'),
      ),
      'Profiles' => array(
        'class_name' => 'ProfileMigration',
        'description' => t('Migration of shared profile nodes from Drupal 7'),
        'machine_name' => 'shared_profile',
        'source_type' => 'shared_profile',
        'destination_type' => 'shared_profile',
        'group_name' => 'shared_content',
        'source_version' => 7,
        'source_connection' => 'shared_migrate',
      ),
      'Scholarships' => array(
        'class_name' => 'ScholarshipMigration',
        'description' => t('Migration of shared scholarships from Drupal 7'),
        'machine_name' => 'shared_scholarship',
        'source_type' => 'shared_scholarship',
        'destination_type' => 'shared_scholarship',
        'group_name' => 'shared_content',
        'source_version' => 7,
        'source_connection' => 'shared_migrate',
      ),
      'Concentrations' => array(
        'class_name' => 'ConcentrationMigration',
        'description' => t('Migration of shared concentrations from Drupal 7'),
        'machine_name' => 'shared_concentration',
        'source_type' => 'shared_concentration',
        'destination_type' => 'shared_concentration',
        'group_name' => 'shared_content',
        'source_version' => 7,
        'source_connection' => 'shared_migrate',
      ),
      /*'shared_category' => array(
        'class_name' => 'DrupalTerm7Migration',
        'description' => t('Migration of Shared Category Terms from Shared'),
        'machine_name' => 'shared_category',
        'source_vocabulary' => 'shared_category',
        'destination_vocabulary' => 'shared_category',
        'group_name' => 'shared_taxonomies',
        'source_version' => 7,
        'source_connection' => 'shared_migrate',
      ),
      'shared_event_type' => array(
        'class_name' => 'DrupalTerm7Migration',
        'description' => t('Migration of Shared Event Type Terms from Shared'),
        'machine_name' => 'shared_event_type',
        'source_vocabulary' => 'shared_event_type',
        'destination_vocabulary' => 'shared_event_type',
        'group_name' => 'shared_taxonomies',
        'source_version' => 7,
        'source_connection' => 'shared_migrate',
      ),
      'shared_faculty_oversight' => array(
        'class_name' => 'DrupalTerm7Migration',
        'description' => t('Migration of Shared Faculty Oversight Terms from Shared'),
        'machine_name' => 'shared_faculty_oversight',
        'source_vocabulary' => 'shared_faculty_oversight',
        'destination_vocabulary' => 'shared_faculty_oversight',
        'group_name' => 'shared_taxonomies',
        'source_version' => 7,
        'source_connection' => 'shared_migrate',
      ),
      'shared_fund_category' => array(
        'class_name' => 'DrupalTerm7Migration',
        'description' => t('Migration of Shared Fund Category Terms from Shared'),
        'machine_name' => 'shared_fund_category',
        'source_vocabulary' => 'shared_fund_category',
        'destination_vocabulary' => 'shared_fund_category',
        'group_name' => 'shared_taxonomies',
        'source_version' => 7,
        'source_connection' => 'shared_migrate',
      ),
      'shared_news_category' => array(
        'class_name' => 'DrupalTerm7Migration',
        'description' => t('Migration of Shared News Category Terms from Shared'),
        'machine_name' => 'shared_news_category',
        'source_vocabulary' => 'shared_news_category',
        'destination_vocabulary' => 'shared_news_category',
        'group_name' => 'shared_taxonomies',
        'source_version' => 7,
        'source_connection' => 'shared_migrate',
      ),
      'shared_profile_type' => array(
        'class_name' => 'DrupalTerm7Migration',
        'description' => t('Migration of Shared Profile Type Terms from Shared'),
        'machine_name' => 'shared_profile_type',
        'source_vocabulary' => 'shared_profile_type',
        'destination_vocabulary' => 'shared_profile_type',
        'group_name' => 'shared_taxonomies',
        'source_version' => 7,
        'source_connection' => 'shared_migrate',
      ),
      'shared_scholarship_category' => array(
        'class_name' => 'DrupalTerm7Migration',
        'description' => t('Migration of Shared Scholarship Category Terms from Shared'),
        'machine_name' => 'shared_scholarship_category',
        'source_vocabulary' => 'shared_scholarship_category',
        'destination_vocabulary' => 'shared_scholarship_category',
        'group_name' => 'shared_taxonomies',
        'source_version' => 7,
        'source_connection' => 'shared_migrate',
      ),
      'background_images' => array(
        'class_name' => 'DrupalTerm7Migration',
        'description' => t('Migration of Shared Background Image Terms from Shared'),
        'machine_name' => 'background_images',
        'source_vocabulary' => 'background_images',
        'destination_vocabulary' => 'background_images',
        'group_name' => 'shared_taxonomies',
        'source_version' => 7,
        'source_connection' => 'shared_migrate',
      ),
      'background_colors' => array(
        'class_name' => 'DrupalTerm7Migration',
        'description' => t('Migration of Shared Background Color Terms from Shared'),
        'machine_name' => 'background_colors',
        'source_vocabulary' => 'background_colors',
        'destination_vocabulary' => 'background_colors',
        'group_name' => 'shared_taxonomies',
        'source_version' => 7,
        'source_connection' => 'shared_migrate',
      ),
      'tags' => array(
        'class_name' => 'DrupalTerm7Migration',
        'description' => t('Migration of Tags from Shared'),
        'machine_name' => 'tags',
        'source_vocabulary' => 'tags',
        'destination_vocabulary' => 'tags',
        'group_name' => 'shared_taxonomies',
        'source_version' => 7,
        'source_connection' => 'shared_migrate',
      ),*/
    ),
  );
  return $api;
}
//function migrate_d2d_shared_register_migrations() {

  // Register the user migration.
  // We just use the migrate_d2d D6 migration class as-is.
  /*$arguments = $common_arguments + array(
    'description' => t('Migration of users from Drupal 7'),
    'machine_name' => 'User',
    'class_name' => 'DrupalUser7Migration',
  );*/

  // For vocabulary migrations, source_vocabulary and destination_vocabulary are
  // required arguments. Note that in Drupal 6 vocabularies did not have machine
  // names, so we use the vocabulary ID to uniquely identify them.
 /* $vocabulary_arguments = array(
    'shared_category' => array(
      'description' => t('Migration of Shared Category Terms from Shared'),
      'source_vocabulary' => 'shared_category',
      'destination_vocabulary' => 'shared_category',
      'group_name' => 'shared_taxonomies',
    ),
    'shared_event_type' => array(
      'description' => t('Migration of Shared Event Type Terms from Shared'),
      'source_vocabulary' => 'shared_event_type',
      'destination_vocabulary' => 'shared_event_type',
      'group_name' => 'shared_taxonomies',
    ),
    'shared_faculty_oversight' => array(
      'description' => t('Migration of Shared Faculty Oversight Terms from Shared'),
      'source_vocabulary' => 'shared_faculty_oversight',
      'destination_vocabulary' => 'shared_faculty_oversight',
      'group_name' => 'shared_taxonomies',
    ),
    'shared_fund_category' => array(
      'description' => t('Migration of Shared Fund Category Terms from Shared'),
      'source_vocabulary' => 'shared_fund_category',
      'destination_vocabulary' => 'shared_fund_category',
      'group_name' => 'shared_taxonomies',
    ),
    'shared_news_category' => array(
      'description' => t('Migration of Shared News Category Terms from Shared'),
      'source_vocabulary' => 'shared_news_category',
      'destination_vocabulary' => 'shared_news_category',
      'group_name' => 'shared_taxonomies',
    ),
    'shared_profile_type' => array(
      'description' => t('Migration of Shared Profile Type Terms from Shared'),
      'source_vocabulary' => 'shared_profile_type',
      'destination_vocabulary' => 'shared_profile_type',
      'group_name' => 'shared_taxonomies',
    ),
    'shared_scholarship_category' => array(
      'description' => t('Migration of Shared Scholarship Category Terms from Shared'),
      'source_vocabulary' => 'shared_scholarship_category',
      'destination_vocabulary' => 'shared_scholarship_category',
      'group_name' => 'shared_taxonomies',
    ),
    'tags' => array(
      'description' => t('Migration of Tags from Shared'),
      'source_vocabulary' => 'tags',
      'destination_vocabulary' => 'tags',
      'group_name' => 'shared_taxonomies',
    ),
  );

  // Again, we're using the migrate_d2d class directly.
  // The soft dependency says that while we don't have to run the user migration
  // first, we want to make sure it's listed first so the vocabularies are
  // listed right ahead of the node migrations.
  $common_vocabulary_arguments = $common_arguments + array(
    'class_name' => 'DrupalTerm7Migration',
    //'soft_dependencies' => array('User'),
  );
  foreach ($vocabulary_arguments as $migration_name => $arguments) {
    $arguments += $common_vocabulary_arguments;
    $api['migrations'][$migration_name] = $arguments;
  }*/

  // Node migrations - each has its own class derived from the migrate_d2d class,
  // specifying its particular field mappings and transformations. source_type
  // and destination_type are required arguments.
 /* $node_arguments = array(
    array(
      'class_name' => 'NewsMigration',
      'description' => t('Migration of shared news nodes from Drupal 7'),
      'machine_name' => 'shared_news',
      'source_type' => 'shared_news',
      'destination_type' => 'shared_news',
      'group_name' => 'shared_content',
    ),
    array(
      'class_name' => 'EventMigration',
      'description' => t('Migration of shared event nodes from Drupal 7'),
      'machine_name' => 'shared_event',
      'source_type' => 'shared_event',
      'destination_type' => 'shared_event',
      'group_name' => 'shared_content',
    ),
    /*'Profiles' => array(
      'class_name' => 'ProfileMigration',
      'description' => t('Migration of shared profile nodes from Drupal 7'),
      'source_type' => 'shared_profile',
      'destination_type' => 'shared_profile',
      'group_name' => 'shared_content',
    ),*/
  /*);

  // Tell the node migrations where the users are coming from, so they can
  // set up the dependency and resolve D6->D7 uids.
  /*$common_node_arguments = $common_arguments + array(
    'user_migration' => 'User'
  );*/

/*  foreach ($node_arguments as $arguments) {
    //$arguments = array_merge_recursive($arguments, $common_node_arguments);
    Migration::registerMigration($arguments['class_name'], $arguments['machine_name'], $arguments);
  }

  // Register the file migration.
  // source_dir needs to be adjusted to match your file system.
  /*$api['migrations']['File'] = $common_arguments + array(
    'File Migration' => array(
      'description' => t('Import Drupal 7 files'),
      'class_name' => 'DrupalFile7Migration',
      'user_migration' => 'User',
      'default_uid' => 1,
      'source_dir' => DRUPAL_ROOT . '/../drupal-6.x-dev',
      'destination_dir' => 'public://shared_migrate',
    ),*/
/*}*/
