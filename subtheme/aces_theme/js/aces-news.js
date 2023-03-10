jQuery(function ($) {
  $.each(".news--aces").click(function() {
    window.location = $(this).find("a").attr("href");
    return false;
  });
});
