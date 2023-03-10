jQuery(function ($) {
  if($('.region-content > div.il-content-with-section-nav > div.il-content').length) {
    $('#block-aces-theme-page-title').prependTo('.region-content > div.il-content-with-section-nav > div.il-content');
    $('#block-aces-theme-breadcrumbs').prependTo('.region-content > div.il-content-with-section-nav > div.il-content');
  } else {

  }
});
