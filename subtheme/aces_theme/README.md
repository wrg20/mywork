# ACES Theme Install
Installing the ACES subtheme on the Illinois Framework

## Make sure to include the following composer.json file in the root of your theme.
```
{
    "name": "aces-wms/aces_theme",
    "description": "College of ACES web theme.",
    "type": "drupal-custom-theme",
    "require": {}
}
```
### Installation with Script
<ol>
<li>Run the site-build.sh script<br/>

```
#!/bin/bash
COMPOSER_MEMORY_LIMIT=-1 composer create-project --remove-vcs --repository="{\"url\": \"https://github.com/web-illinois/illinois_framework_project.git\", \"type\": \"vcs\"}" web-illinois/illinois_framework_project:1.x-dev my-fw-project
ln -s ~/illinois_framework/vendor ~/vendor
ln -s ~/illinois_framework/docroot/.* ~/public_html/
ln -s ~/illinois_framework/docroot/* ~/public_html/
```
<li>Add the custom theme<br/>

```
cd ~/illinois_framework/

composer config repositories.aces-theme '{"type": "vcs", "url": "https://github.com/aces-wms/aces-theme.git"}'
    
composer require aces-wms/aces_theme:^1.0

```

</li>
<li>Navigate to /admin/appearance and install the ACES theme</li>
    > Select "install and set as default".  The illinois framework is the default theme.
