# This is a basic VCL configuration file for varnish.  See the vcl(7)
# man page for details on VCL syntax and semantics.
#
# Included for Monit to check if Varnish is running or not
include "/etc/varnish/monit.vcl";
# 
# Default backend definition.  Set this to point to your content
# server.
# 
backend default {
    .host = "127.0.0.1";
    .port = "8080";
    .connect_timeout = 60s;
    .first_byte_timeout = 60s;
    .between_bytes_timeout = 60s;
}
acl internal {
  "172.16.12.0"/20;
}


# 
# Below is a commented-out copy of the default VCL logic.  If you
# redefine any of these subroutines, the built-in logic will be
# appended to your code.

include "devicedetect.vcl";

sub vcl_recv {
	call devicedetect;
 
# Do not cache these paths.
  if (req.url ~ "^/status\.php$" && client.ip ~ internal ||
      req.url ~ "^/update\.php$" && client.ip ~ internal ||
      req.url ~ "^/ooyala/ping$" && client.ip ~ internal ||
      req.url ~ "^/admin$" && client.ip ~ internal ||
      req.url ~ "^/admin/.*$" && client.ip ~ internal ||
      req.url ~ "^/info/.*$" && client.ip ~ internal ||
      req.url ~ "^/pdfs/.*$" && client.ip ~ internal ||
      req.url ~ "^/flag/.*$" && client.ip ~ internal ||
      req.url ~ "^.*/ajax/.*$" && client.ip ~ internal ||
      req.url ~ "^.*/db/.*$" && client.ip ~ internal ||
      req.url ~ "^.*/wwwlog/.*$" && client.ip ~ internal ||
      req.url ~ "^/apc.php" && client.ip ~ internal ||
      req.url ~ "^/memcache.php" && client.ip ~ internal ||
      req.url ~ "^.*/ahah/.*$") {	
	return (pass);
  }

# Use anonymous, cached pages if all backends are down.
  if (!req.backend.healthy) {
    unset req.http.Cookie;
  }

  # Allow the backend to serve up stale content if it is responding slowly.
  set req.grace = 6h;

# Do not allow outside access to cron.php or install.php.
  if (req.url ~ "^/(cron|install|phpinfo|memcache|authorize|update|apc|xmlrpc)\.*.*$" && !client.ip ~ internal ||
      req.url ~ "^/((?i)user|admin|node)" && !client.ip ~ internal ||
      req.url ~ "^/(?i)wwwlog" && !client.ip ~ internal ||
      req.url ~ "^/(?i)db" && !client.ip ~ internal ||
      req.url ~ "^/(?i)doc" && !client.ip ~ internal ||
      req.url ~ "(/node/\d+/edit|add)" && !client.ip ~ internal ||
      req.url ~ "^/((?i)README|CHANGELOG|INSTALL|LICENSE|MAINTAINERS|UPGRADE|COPYRIGHT).*.*$" && !client.ip ~ internal ||   
      req.url ~ "^/(?i)web.config" && !client.ip ~ internal 
      #req.url ~ "^/.git/*" && !client.ip ~ internal
      ){
    # Have Varnish throw the error directly.
    # error 404 "Page not found.";
    # Use a custom error page that you've defined in Drupal at the path "404".
    set req.url = "/page-not-found";
  }

# Pipe these paths directly to Apache for streaming.
  if (req.url ~ "^/admin/content/backup_migrate/export") {
    return (pipe);
  }
# Handle compression correctly. Different browsers send different
  # "Accept-Encoding" headers, even though they mostly all support the same
  # compression mechanisms. By consolidating these compression headers into
  # a consistent format, we can reduce the size of the cache and get more hits.=
  # @see: http:// varnish.projects.linpro.no/wiki/FAQ/Compression
  if (req.http.Accept-Encoding) {
    if (req.http.Accept-Encoding ~ "gzip") {
      # If the browser supports it, we'll use gzip.
      set req.http.Accept-Encoding = "gzip";
    }
    else if (req.http.Accept-Encoding ~ "deflate") {
      # Next, try deflate if it is supported.
      set req.http.Accept-Encoding = "deflate";
    }
    else {
      # Unknown algorithm. Remove it and send unencoded.
      unset req.http.Accept-Encoding;
    }
  }
# Always cache the following file types for all users.
  if (req.url ~ "(?i)\.(png|gif|jpeg|jpg|ico|swf|css|js|html|htm)(\?[a-z0-9]+)?$") {
    unset req.http.Cookie;
  }
# Remove all cookies that Drupal doesn't need to know about. ANY remaining
  # cookie will cause the request to pass-through to Apache. For the most part
  # we always set the NO_CACHE cookie after any POST request, disabling the
  # Varnish cache temporarily. The session cookie allows all authenticated users
  # to pass through as long as they're logged in.
  if (req.http.Cookie) {
    set req.http.Cookie = ";" + req.http.Cookie;
    set req.http.Cookie = regsuball(req.http.Cookie, "; +", ";");
    set req.http.Cookie = regsuball(req.http.Cookie, ";(SESS[a-z0-9]+|NO_CACHE)=", "; \1=");
    set req.http.Cookie = regsuball(req.http.Cookie, ";[^ ][^;]*", "");
    set req.http.Cookie = regsuball(req.http.Cookie, "^[; ]+|[; ]+$", "");

    if (req.http.Cookie == "") {
      # If there are no remaining cookies, remove the cookie header. If there
      # aren't any cookie headers, Varnish's default behavior will be to cache
      # the page.
      unset req.http.Cookie;
    }
    else {
      # If there is any cookies left (a session or NO_CACHE cookie), do not
      # cache the page. Pass it on to Apache directly.
      return (pass);
    }
  }



   if (req.restarts == 0) {
 	if (req.http.x-forwarded-for) {
 	    set req.http.X-Forwarded-For =
 		req.http.X-Forwarded-For + ", " + client.ip;
 	} else {
 	    set req.http.X-Forwarded-For = client.ip;
 	}
    }
     if (req.request != "GET" &&
       req.request != "HEAD" &&
       req.request != "PUT" &&
       req.request != "POST" &&
       req.request != "TRACE" &&
       req.request != "OPTIONS" &&
       req.request != "DELETE") {
         /* Non-RFC2616 or CONNECT which is weird. */
         return (pipe);
     }
     if (req.request != "GET" && req.request != "HEAD") {
         /* We only deal with GET and HEAD by default */
         return (pass);
     }

#     if (req.http.Authorization || req.http.Cookie) {
#         /* Not cacheable by default */
#         return (pass);
#     }
     
     if (!client.ip ~ internal){
	 
     if (req.http.user-agent ~ "^$"
      || req.http.user-agent ~ "^Java"
      || req.http.user-agent ~ "^Jakarta"
      || req.http.user-agent ~ "IDBot"
      || req.http.user-agent ~ "id-search"
      || req.http.user-agent ~ "User-Agent"
      || req.http.user-agent ~ "compatible ;"
      || req.http.user-agent ~ "ConveraCrawler"
      || req.http.user-agent ~ "^Mozilla$"
      || req.http.user-agent ~ "libwww"
      || req.http.user-agent ~ "lwp-trivial"
      || req.http.user-agent ~ "curl"
      || req.http.user-agent ~ "PHP/"
      || req.http.user-agent ~ "urllib"
      || req.http.user-agent ~ "urllib2"
      || req.http.user-agent ~ "GT:WWW"
      || req.http.user-agent ~ "Snoopy"
      || req.http.user-agent ~ "MFC_Tear_Sample"
      || req.http.user-agent ~ "HTTP::Lite"
      || req.http.user-agent ~ "PHPCrawl"
      || req.http.user-agent ~ "URI::Fetch"
      || req.http.user-agent ~ "Zend_Http_Client"
      || req.http.user-agent ~ "http client"
      || req.http.user-agent ~ "PECL::HTTP"
      || req.http.user-agent ~ "panscient.com"
      || req.http.user-agent ~ "IBM EVV"
      || req.http.user-agent ~ "Bork-edition"
      || req.http.user-agent ~ "Fetch API Request"
      || req.http.user-agent ~ "PleaseCrawl"
      || req.http.user-agent ~ "[A-Z][a-z]{3,} [a-z]{4,} [a-z]{4,}"
      || req.http.user-agent ~ "layeredtech.com"
      || req.http.user-agent ~ "WEP Search"
      || req.http.user-agent ~ "Wells Search II"
      || req.http.user-agent ~ "Missigua Locator"
      || req.http.user-agent ~ "ISC Systems iRc Search 2.1"
      || req.http.user-agent ~ "Microsoft URL Control"
      || req.http.user-agent ~ "Indy Library"
      || req.http.user-agent == "8484 Boston Project v 1.0"
      || req.http.user-agent == "Atomic_Email_Hunter/4.0"
      || req.http.user-agent == "atSpider/1.0"
      || req.http.user-agent == "autoemailspider"
      || req.http.user-agent == "China Local Browse 2.6"
      || req.http.user-agent == "ContactBot/0.2"
      || req.http.user-agent == "ContentSmartz"
      || req.http.user-agent == "DataCha0s/2.0"
      || req.http.user-agent == "DataCha0s/2.0"
      || req.http.user-agent == "DBrowse 1.4b"
      || req.http.user-agent == "DBrowse 1.4d"
      || req.http.user-agent == "Demo Bot DOT 16b"
      || req.http.user-agent == "Demo Bot Z 16b"
      || req.http.user-agent == "DSurf15a 01"
      || req.http.user-agent == "DSurf15a 71"
      || req.http.user-agent == "DSurf15a 81"
      || req.http.user-agent == "DSurf15a VA"
      || req.http.user-agent == "EBrowse 1.4b"
      || req.http.user-agent == "Educate Search VxB"
      || req.http.user-agent == "EmailSiphon"
      || req.http.user-agent == "EmailWolf 1.00"
      || req.http.user-agent == "ESurf15a 15"
      || req.http.user-agent == "ExtractorPro"
      || req.http.user-agent == "Franklin Locator 1.8"
      || req.http.user-agent == "FSurf15a 01"
      || req.http.user-agent == "Full Web Bot 0416B"
      || req.http.user-agent == "Full Web Bot 0516B"
      || req.http.user-agent == "Full Web Bot 2816B"
      || req.http.user-agent == "Guestbook Auto Submitter"
      || req.http.user-agent == "Industry Program 1.0.x"
      || req.http.user-agent == "ISC Systems iRc Search 2.1"
      || req.http.user-agent == "IUPUI Research Bot v 1.9a"
      || req.http.user-agent == "LARBIN-EXPERIMENTAL (efp@gmx.net)"
      || req.http.user-agent == "LetsCrawl.com/1.0 +http://letscrawl.com/"
      || req.http.user-agent == "Lincoln State Web Browser"
      || req.http.user-agent == "LMQueueBot/0.2"
      || req.http.user-agent == "LWP::Simple/5.803"
      || req.http.user-agent == "Mac Finder 1.0.xx"
      || req.http.user-agent == "MFC Foundation Class Library 4.0"
      || req.http.user-agent == "Microsoft URL Control - 6.00.8xxx"
      || req.http.user-agent == "Missauga Locate 1.0.0"
      || req.http.user-agent == "Missigua Locator 1.9"
      || req.http.user-agent == "Missouri College Browse"
      || req.http.user-agent == "Mizzu Labs 2.2"
      || req.http.user-agent == "Mo College 1.9"
      || req.http.user-agent == "Mozilla/2.0 (compatible; NEWT ActiveX; Win32)"
      || req.http.user-agent == "Mozilla/3.0 (compatible; Indy Library)"
      || req.http.user-agent == "Mozilla/4.0 (compatible; Advanced Email Extractor v2.xx)"
      || req.http.user-agent == "Mozilla/4.0 (compatible; Iplexx Spider/1.0 http://www.iplexx.at)"
      || req.http.user-agent == "Mozilla/4.0 (compatible; MSIE 5.0; Windows NT; DigExt; DTS Agent"
      || req.http.user-agent == "Mozilla/4.0 efp@gmx.net"
      || req.http.user-agent == "Mozilla/5.0 (Version: xxxx Type:xx)"
      || req.http.user-agent == "MVAClient"
      || req.http.user-agent == "NameOfAgent (CMS Spider)"
      || req.http.user-agent == "NASA Search 1.0"
      || req.http.user-agent == "Nsauditor/1.x"
      || req.http.user-agent == "PBrowse 1.4b"
      || req.http.user-agent == "PEval 1.4b"
      || req.http.user-agent == "Poirot"
      || req.http.user-agent == "Port Huron Labs"
      || req.http.user-agent == "Production Bot 0116B"
      || req.http.user-agent == "Production Bot 2016B"
      || req.http.user-agent == "Production Bot DOT 3016B"
      || req.http.user-agent == "Program Shareware 1.0.2"
      || req.http.user-agent == "PSurf15a 11"
      || req.http.user-agent == "PSurf15a 51"
      || req.http.user-agent == "PSurf15a VA"
      || req.http.user-agent == "psycheclone"
      || req.http.user-agent == "RSurf15a 41"
      || req.http.user-agent == "RSurf15a 51"
      || req.http.user-agent == "RSurf15a 81"
      || req.http.user-agent == "searchbot admin@google.com"
      || req.http.user-agent == "ShablastBot 1.0"
      || req.http.user-agent == "snap.com beta crawler v0"
      || req.http.user-agent == "Snapbot/1.0"
      || req.http.user-agent == "sogou develop spider"
      || req.http.user-agent == "Sogou Orion spider/3.0(+http://www.sogou.com/docs/help/webmasters.htm#07)"
      || req.http.user-agent == "sogou spider"
      || req.http.user-agent == "Sogou web spider/3.0(+http://www.sogou.com/docs/help/webmasters.htm#07)"
      || req.http.user-agent == "sohu agent"
      || req.http.user-agent == "SSurf15a 11"
      || req.http.user-agent == "TSurf15a 11"
      || req.http.user-agent == "Under the Rainbow 2.2"
      || req.http.user-agent == "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)"
      || req.http.user-agent == "VadixBot"
      || req.http.user-agent == "WebVulnCrawl.blogspot.com/1.0 libwww-perl/5.803"
      || req.http.user-agent == "Wells Search II"
      || req.http.user-agent == "WEP Search 00"
      || req.http.user-agent == "123peoplebot/1.0"
      || req.http.user-agent == "Zeus"	
      || req.http.User-Agent == "Wget") {
 		error 403 "You are Blocked from accessing this site. To gain access to this site please call the SURS webmaster at 800-ASK-SURS";
         } 
     }
     return (lookup);
 }
# 
 sub vcl_pipe {
#     # Note that only the first request to the backend will have
#     # X-Forwarded-For set.  If you use X-Forwarded-For and want to
#     # have it set for all requests, make sure to have:
#     # set bereq.http.connection = "close";
#     # here.  It is not set by default as it might break some broken web
#     # applications, like IIS with NTLM authentication.
     set bereq.http.X-Forwarded-For = req.http.X-Forwarded-For;
     set bereq.http.X-Forwarded-For = regsub(bereq.http.X-Forwarded-For,"$", ", ");
     set bereq.http.X-Forwarded-For = regsub(bereq.http.X-Forwarded-For,"$", client.ip);
     set bereq.http.connection = "close";
     return (pipe);
 }
# 
 sub vcl_pass {
     if (req.http.X-UA-Device) {
        set bereq.http.User-Agent = req.http.X-UA-Device;   
     }
 }
# 
# sub vcl_hash {
#     hash_data(req.url);
#     if (req.http.host) {
#         hash_data(req.http.host);
#     } else {
#         hash_data(server.ip);
#     }
#     return (hash);
# }
# 
# sub vcl_hit {
#     return (deliver);
# }
# 
 sub vcl_miss {
     if (req.http.X-UA-Device) {
	set bereq.http.User-Agent = req.http.X-UA-Device;
     } 
 }
# 
sub vcl_fetch {
  
  # Don't allow static files to set cookies.
  if (req.url ~ "(?i)\.(png|gif|jpeg|jpg|ico|swf|css|pdf|js|html|htm)(\?[a-z0-9]+)?$") {
    # beresp == Back-end response from the web server.
    unset beresp.http.set-cookie;
  }

  # Remove the X-Forwarded-For header if it exists
  remove req.http.X-Forwarded-For;
  
  # insert the client IP address as X-Forwarded-For
  set req.http.X-Forwarded-For = req.http.rlnclientipaddr;

  # Added security the "w00tw00t" attack is really annoying
  if (req.url ~ "^/w00tw00t" || 
      req.url ~ "^/(?i)phpmyadmin" ||
      req.url ~ "^/(?i)pma" ||
      req.url ~ "^/(?i)databases" ||
      req.url ~ "^/(?i)Toata"){
	error 403 "Not permitted";
  }

  # Remove Server Header and Replace with something else
  #unset resp.http.Server;
  #set resp.http.Server = "SURS Server";

  # Allow items to be stale if needed.
  set beresp.grace = 6h;

# Varnish determined the object was not cacheable
    if (beresp.ttl <= 0s) {
        set beresp.http.X-Varnish-Cacheable = "NO:Not Cacheable";
    
    # You don't wish to cache content for logged in users
    } elsif (req.http.Cookie ~ "(some_cookie_name)") {
        set beresp.http.X-Varnish-Cacheable = "NO:Got Session";
        return(hit_for_pass);
    
    # You are respecting the Cache-Control=private header from the backend
    } elsif (beresp.http.Cache-Control ~ "private") {
        set beresp.http.X-Varnish-Cacheable = "NO:Cache-Control=private";
        return(hit_for_pass);
    
    # Varnish determined the object was cacheable
    } else {
        set beresp.http.X-Varnish-Cacheable = "YES";
    }
    
    # ....
    return(deliver);
    
    if (req.http.X-UA-Device) {
	if (!beresp.http.Vary) { 
		#no Vary at all
		set beresp.http.Vary = "X-UA-Device";
	} elseif (beresp.http.Vary !~ "X-UA-Device") { 
		# add to existing Vary
		set beresp.http.Vary = beresp.http.Vary + ", X-UA-Device";
	}
    }
    set beresp.http.X-UA-Device = req.http.X-UA-Device;
}

# In the event of an error, show friendlier messages.
sub vcl_error {
  # Redirect to some other URL in the case of a homepage failure.
  #if (req.url ~ "^/?$") {
  #  set obj.status = 302;
  #  set obj.http.Location = "http://backup.example.com/";
  #}

  # Otherwise redirect to the homepage, which will likely be in the cache.
  set obj.http.Content-Type = "text/html; charset=utf-8";
  synthetic {"
<html>
<head>
  <title>Page Unavailable</title>
  <style>
    body { background: #303030; text-align: center; color: white; }
    #page { border: 1px solid #CCC; width: 500px; margin: 100px auto 0; padding: 30px; background: #323232; }
    a, a:link, a:visited { color: #CCC; }
    .error { color: #222; }
  </style>
</head>
<body onload="setTimeout(function() { window.location = '/' }, 5000)">
  <div id="page">
    <h1 class="title">Page Unavailable</h1>
    <p>The page you requested is temporarily unavailable.</p>
    <p>We're redirecting you to the <a href="/">homepage</a> in 5 seconds.</p>
    <p>If you would like to go directly to the member website <a href="/login">Click Here</a></p>
    <div class="error">(Error "} + obj.status + " " + obj.response + {")</div>
  </div>
</body>
</html>
"};
  return (deliver);
}

sub vcl_deliver {
  if ((req.http.X-UA-Device) && (resp.http.Vary)){
	set resp.http.Vary = regsub(resp.http.Vary, "X-UA-Device", "User-Agent");
  }

  if (obj.hits > 0) {
    set resp.http.X-Varnish-Cache = "HIT";
  }
  else {
    set resp.http.X-Varnish-Cache = "MISS";
  }
  
  # Remove Server Header and Replace with something else
  unset resp.http.Server;
  set resp.http.Server = "SURS Server";
  set resp.http.X-Content-Type-Options = "nosniff"; 
  set resp.http.X-Frame-Options = "SAMEORIGIN";
  set resp.http.X-UA-Compatible = "IE=Edge";
  
  #remove resp.http.X-Content-Type-Options;
  #remove resp.http.X-Frame-Options;
  remove resp.http.X-Varnish;
  remove resp.http.Via;
  remove resp.http.Age;
  remove resp.http.X-Powered-By;
  remove resp.http.X-Drupal-Cache;
  remove resp.http.X-Generator;
  remove resp.http.ETag;

  return (deliver);
}
# 
# 
# sub vcl_init {
# 	return (ok);
# }
# 
# sub vcl_fini {
# 	return (ok);
# }
                     
