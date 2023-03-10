# Lando Dev Environment Setup for Windows Users
How to setup Development Environment in Lando with [Docker Desktop for Windows](https://hub.docker.com/editions/community/docker-ce-desktop-windows)

## Installs
* requires you to install [gitbash for windows](https://git-scm.com/downloads)
* requires you to install [composer for windows](https://getcomposer.org/download/) globally 
* requires you to install [drush for windows](https://www.drupal.org/node/594744) globally 
  > Running `composer global require drush/drush:10.*` from the command line using [gitbash](https://git-scm.com/).  If you are unable to install drush 10 please refer to the [installing php for windows section](## Upgrading windows 10 php version) below.  You may need to upgrade to PHP 7.4+ and add it to your system environment variables PATH.

## Setting up aliases
* Add paths to .bashrc or system environment variables (.bashrc should exist in the /c/Users/[USERNAME] folder)
* Type `which drush` in the Gitbash CLI.  You should see the path where drush exists.  Add that path to your .bashrc file as an alias.
  >`alias drush='/c/Users/[USERNAME]/AppData/Roaming/Composer/vendor/bin/drush'`
* Type `which composer` into the GitBash CLI.  You should see the path where composer exists.  Add that path to your .bashrc files as an alias.
  >`alias composer="COMPOSER_MEMORY_LIMIT=-1 /c/ProgramData/ComposerSetup/bin/composer.phar"`

## Windows installation 
1. Download and install [Lando](https://lando.dev/download/)
2. Download and install [Docker Desktop](https://hub.docker.com/editions/community/docker-ce-desktop-windows)
3. Enable WSL 2 if able on your system
4. Open GitBash
  * Create a directory called `web` in your USER directory( /c/User/[USERNAME]) `mkdir web`
  * Create a directory within `web` for your development environments `mkdir dev`
5. Move within the `dev` directory and run `git init`
6. Followed by `git clone https://github.com/aces-wms/lando_dev_environment.git`
  > if you are prompted for a token. Copy and paste the URL into your browser and generate a token in your Github Personal access tokens area (Settings=>Developer Settings=>Personal Aaccess Tokens).  You must Enable SSO for web-illinois before pasting the token into the CLI and proceeding.
  > ![image](https://user-images.githubusercontent.com/1544079/116645384-5b9b4880-a93b-11eb-9207-44fc2a042f17.png)
  > You will need access to the repository in order to clone it.
7. Once the repository has cloned then rename the lando_dev_environment folder to your site name `mv lando_dev_environment your.site.illinois.edu`
8. `cd your.site.illinois.edu` and run `lando init` from the root directory (/c/Users/[USERNAME]/web/dev/your.site.illinois.edu/)
9. `sh site-build.sh` Run the site-build script from the root directory (/c/Users/[USERNAME]/web/dev/your.site.illinois.edu/)
  > If this does not work refer to the [composer alias](## Setting up aliases) above regarding memory issues
10. Within the site root directory (/c/Users/[USERNAME]/web/dev/your.site.illinois.edu) run `lando start`
  > This process should kick of Docker to start up and may take just a couple of minutes while drush, php, apache, mariadb, etc are downloaded and installed.
  > Take note of the APPSERVER URLS, specifically the https://your.site.lndo.site URL. This is your URL.
11. Now that your server is running, install your framework by running the site install script `sh site-install.sh`
  > This process could take up to 15-20 minutes as the framework is installed depending on your local computer resources.  
12. Go to your website and login with the admin username and password given at the end of the install


## Import a database from an existing website on the same environment
18. Import a database by downloading the database from the [AWS S3 bucket](https://aws.illinois.edu/) 
    > Place the downloaded file within the project root directory (i.e. /c/Users/[USERNAME]/web/dev/dev.ansc.illinois.edu)
19. Run the lando db-import command to pull in the database by using `lando db-import *.sql.gz`


## Install modules using composer
1. Disable TLS on development machine by running `composer config -g -- disable-tls true` within the directory of the composer.json file you want to update

## TODOS
1. Add alias files for ACES websites
2. Create a script to complete most of these steps automatically using the site alias
3. Install instructions for AWS CLI on windows and [setting up aws configure with Access keys](https://docs.aws.amazon.com/cli/latest/userguide/cli-configure-quickstart.html) 

## Upgrading windows 10 php version
https://windows.php.net/download/
* See [install PHP7.4](https://dev.to/amulya_shahi/how-to-download-install-php-7-4-6-manually-on-windows-10-4io0)
  > [PHP 7.4.18 non-thread safe for 64 bit](https://windows.php.net/downloads/releases/php-7.4.18-nts-Win32-vc15-x64.zip)
  > ![image](https://user-images.githubusercontent.com/1544079/116642512-b41b1780-a934-11eb-865e-ea837b086318.png)
  > ![image](https://user-images.githubusercontent.com/1544079/116642662-00feee00-a935-11eb-9140-5be52f6e450e.png)
* Enable extensions
![image](https://user-images.githubusercontent.com/1544079/116642862-5f2bd100-a935-11eb-9e72-0b3ce0198cab.png)
* Edit php.ini file in latest version of php on your computer
    > Add extensions gd2, mbstring, pdo_mysql (or whatever database type you'll use) and openssl by uncommenting them in the php.ini file located at C:\Program Files PHP\[PHPVERSION]\php.ini
    > May need to change the extension_dir = "C:\Program Files\PHP\ext" or wherever your version of PHP lives

