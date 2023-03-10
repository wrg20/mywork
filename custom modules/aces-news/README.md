# aces-news

## Installation with composer
<ul>
<li>Add the custom module</li>

```
cd ~/illinois_framework/

composer config repositories.aces-news '{"type": "vcs", "url": "https://github.com/aces-wms/aces-news.git"}'

composer require aces-wms/aces-news:^1.x-dev
```
<li>Add custom module install location to configuration extra.installer-paths</li>

```
"docroot/modules/custom/{$name}": [
      "type:drupal-custom-module"
 ],
```
# aces-news error on reinstall

May require you to delete config for reinstall.
cd into illinois_framework then run the following
```drush config-del core.entity_form_display.node.news.default;
 drush config-del core.entity_view_display.node.news.default;
 drush config-del core.entity_view_display.node.news.news_card;
 drush config-del core.entity_view_mode.node.news_card;
 drush config-del field.field.node.news.body;
 drush config-del field.field.node.news.field_tags;
 drush config-del field.field.node.news.field_if_author;
 drush config-del field.field.node.news.field_if_image_caption;
 drush config-del field.storage.node.field_if_image_caption;
 drush config-del node.news.field_tags;
 drush config-del pathauto.pattern.news_article;
 drush config-del views.view.news;```
