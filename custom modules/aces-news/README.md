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
