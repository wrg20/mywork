# Drupal local development environment instructions
Guide to setting up a College of ACES local development environment (Multisite)

change [php tp 7.2.22]


## Multisite Environment Setup
1. Download Acquia Dev Desktop 2 software. https://dev.acquia.com/downloads
2. Install Git for windows or Mac OS X https://git-scm.com/downloads
3. Install GitHub Desktop https://desktop.github.com/
4. Open AWS and download site archive file
    * Go to aws.illinois.edu and log in
    * Download the latest dev.aces.illinois.edu archive file
        1. Go to All Services->Storage->S3
        2. Select the file (devwms-cpanel->dev.aces.illinois->archives)
        3. Sort by Last modified and select the latest copy (should be today's date) and click on the file
            * example: dev.aces.illinois.edu-05-28-2020-03.00.02.tar
    * Click on the "Download" or "Download as" buttons and follow the prompts to download the file
    * Unzip the archive file and place the contents on your Desktop 
        > Note: You will need zip software such as 7zip https://www.7-zip.org/download.html
5. Create a drupal 8 website in Acquia Dev Desktop
    * Click on the + symbol in the lower left-hand corner 
    * Select "New Drupal site"
    * Click on install for the Drupal 8 distribution
    * Change the defaults to the following
        1. Local codebase folder: C:\Users\[username]\Sites\devdesktop\drupal8
        2. Local site name: drupal8
        3. Click Finish
6. Copy Updated version of drupal core from archive files on Desktop to devdesktop
    1. Copy all files and folders with the exception of .htaccess & /sites
    C:\Users\[username]\Desktop\drupal8\* -> C:\Users\[username]\Sites\devdesktop\drupal8\*
        > Note: if asked about replacing existing files, replace them          
7. Add tmp folder to C:\Users\[username] directory
    1. C:\Users\[username]\tmp                                                                                                                                                                                                                                                                                                                                                                                                                                                                           

## Add a website locally to your computer (example for dev.aces.illinois.edu)
1. Download the archive for the website you want to add to your local environment ([Multisite](#Multisite Environment Setup) - Step 4)
2. Open Acquia Dev Desktop
    * Select the drupal-8-x-x.dd site
    * Click on the More button in the lower left part of the screen
    * Select "New Drupal multisite"
        1. Local site name: dev.aces.illinois.edu
        2. Database: Start with MySQL database dump file
        3. Dump file: C:\Users\[username]\Desktop\devwms_drupal8.dev.aces.sql
        4. New database name: dev_aces_illinois_edu   
        5. Click OK
3. Clone Repository from Github to local computer using Github Desktop  
    1. Open Github Desktop
    2. File->Clone a repository
    3. Choose GitHub.com [will require GitHub.com account] https://github.com
        * Pick the repository from the list
        * Choose the Local path C:\Users\[username]\Documents\GitHub
            > Note: The local path should display as C:\Users\[username]\Documents\GitHub\[repository]
4. Compile the local Sass file so that C:\Users\[username]\Documents\GitHub\dev.aces.illinois.edu\themes\sitetheme\sass to C:\Users\[username]\Documents\GitHub\dev.aces.illinois.edu\themes\sitetheme\css
    * Either use scoutApp to compile locally, install dart-sass locally (see how to compile sass locally)
        > Alternatively copy the styles.css file from C:\Users\[username]\Desktop\drupal8\sites\dev.aces.illinois.edu\themes\sitetheme\css to 
        > C:\Users\[username]\Documents\GitHub\dev.aces.illinois.edu\themes\sitetheme\css                                                 
4. Copy the C:\Users\[username]\Desktop\drupal8\sites\dev.aces.illinois.edu\files directory to C:\Users\[username]\Documents\GitHub\[repository]\files
5. Create tmp folder within C:\Users\[username]\Documents\GitHub\[repository]
    1. C:\Users\[username]\Documents\GitHub\[repository]\tmp
6. Copy the settings.php file from C:\Users\[username]\Sites\devdesktop\drupal8\sites\dev.aces.illinois.edu.dd -> C:\Users\[username]\Documents\GitHub\dev.aces.illinois.edu                                                                                                                                          
7. Create symlink
    1. Open Windows Powershell Administrator Mode (right-click on powershell icon in windows menu and select "Run as Administrator")
    2. Navigate to C:\Users\[username]\Sites\devdesktop\drupal8\sites
    3. Remove the previous dev.aces.illinois.edu.dd symlink
        1. Delete in the windows explorer or type `rm .\dev.aces.illinois.edu.dd\`
            > You may be asked to force the removal in which `rm -force .\dev.aces.illinois.edu.dd\`
            > You may be prompted asking whether you want to continue to recurse through the directory, type y and hit enter                                                                       
        2. Delete in the windows explorer or type `rm .\dev.aces.illinois.edu\`
    4. Create a symlink to the dev.aces.illinois.edu GitHub repository
       * Open Windows Powershell in Administrator Mode
       * Navigate to C:\Users\[username]\Sites\devdesktop\drupal8\sites <br/>
        `New-Item -ItemType SymbolicLink -Path "dev.aces.illinois.edu" -Target "C:\Users\[username]\Documents\GitHub\[repository]\"` <br/>
        `New-Item -ItemType SymbolicLink -Path "dev.aces.illinois.edu.dd" -Target "dev.aces.illinois.edu"`  
8. Settings.php changes
    1. Add the sync directory if it doesn't exist C:\Users\[username]\Documents\GitHub\[sitename]\files\config_HMBjQONao7YcNIJnVma87QsNnVsKEtqihiXIiYCejaRppcbmDy89tPZ1Meo0trgF1iAEhMwwVA\sync
        > This will require you to make two directories within the files directory [config_*] and [config_*\sync]
    2. Add sync directory $config_directories['sync'] = 'C:\Users\[username]\Documents\GitHub\[sitename]\files\config_HMBjQONao7YcNIJnVma87QsNnVsKEtqihiXIiYCejaRppcbmDy89tPZ1Meo0trgF1iAEhMwwVA\sync'; 
    3. Find the devdesktop settings file C:\Users\[username]\.acquia\DevDesktop\DrupalSettings\[sitename].inc (ex: loc_dev_mfst_illinois_edu_dd.inc)
        1. Copy all of the settings from the file into the settings.php file with the exclusion of the <?php ?> tags
            > C:\Users\[username]\Documents\GitHub\alec.illinois.edu\settings.php
        2. Add C:\Users\[username]\Sites\devdesktop\drupal8\sites\dev.aces.illinois.edu\files\[config folder]
            * Then add folders "active" and "staging" to the [config folder]
                > config folder is found in the C:\Users\[username]\.acquia\DevDesktop\DrupalSettings\[sitename].inc (ex: loc_dev_mfst_illinois_edu_dd.inc) file
                > and will look something like config_acqTmp-[date]Txx.xx.xx.-xxx                                                               
    4. Copy the services.yml and settings.local files from C:\Users\[username]\Desktop\drupal8\sites\dev.aces.illinois.edu to C:\Users\[username]\Sites\devdesktop\drupal8\sites\dev.mfst.illinois.edu.dd
    5. Update the $settings['update_free_access'] = FALSE; in the settings.php file to TRUE
        1. Go to https://[sitename].dd:8443/update.php
9. Setup drush alias 
    1. Open the C:\Users\[username]\.drush\aliases.drushrc.php file 
    2. Add the alias for the website like below
`$aliases['loc.dev.aces'] = array(
   'root' => 'C:\Users\[username]\Sites\devdesktop\drupal8',
   'uri' => 'https://dev.aces.illinois.edu.dd:8443',
   '%site' => 'sites/dev.aces.illinois.edu',
   'path-aliases' =>
     array (
         '%drush' => ' C:\Users\[username]\AppData\Roaming\Composer\global\drush\drush\vendor\drush\drush',
         '%dump-dir' => 'C:\Users\[username]\tmp',
     ),
 );`
    3. Open git bash and type in `drush sa` to view the list of aliases
    4. Now perform a cache clear `drush @loc.dev.aces cr`
10. open web browser and accept risk of no certificate
11. Uninstall all ldap modules from the  add configure directory from https://[sitename].dd:8443/admin/modules/uninstall
    * login to the site using the admin username and password
    * go to https://[sitename].dd:8443/admin/modules/uninstall
    * uninstall all ldap enabled modules (3 of them)

 

### Optional Setups
1. MSSQL driver 17 for windows - Install the MSSQL driver 17 for windows. <br/> https://docs.microsoft.com/en-us/sql/connect/odbc/download-odbc-driver-for-sql-server?view=sql-server-ver15
2. Edit php.ini file for PDO connection 
   1. In acquia dev desktop: Open settings → config → php → php7* → php.ini. <br/> ![php config file](install/images/php-config.png)  
   2. Edit the Dev Desktop PHP ini file to allow for pdo_mssql by navigating to preferences, config tab, and selecting 
      edit for the php7.2 php ini file in the list.  Uncomment the line "extension=php_pdo_odbc.dll"
   3. Restart Dev Desktop
        1. Click Stop then Click Start <br/> ![restart acquia dev desktop](install/images/restart-devDesktop.png)

### How to compile Sass
To compile the Sass, there are a number of options, but the simplest is to use PHPStorm to watch
the files and compile upon change. To get setup, follow these instructions inside PHPStorm.

First, you need to npm installed on your local machine. https://nodejs.org/en/download/

After installation, open Powershell or you favorite windows terminal.
```
npm install -global sass
```

- Open Settings and search for File Watchers
- Select the import button on the right hand side of the File Watcher dialog window
- Choose the watchers.xml file found in the sass folder of the EWM project
- The watcher should now compile upon save to a file named style.css
