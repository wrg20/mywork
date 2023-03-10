$(document).ready(function(){
  //fix mega menu to the top of the screen after 173px of scroll
   $(window).bind('scroll', function() {
    if ($(window).scrollTop() > 173) {
      $('#block-tb-megamenu-main-menu').addClass('fixed');
    }
    else {
      $('#block-tb-megamenu-main-menu').removeClass('fixed');
    }
  });
  // remove the link from section navigation items
  //$("#tb-megamenu-main-menu a.dropdown-toggle[href='#']").removeAttr("href");
  // add a down arrow to the megamenu on the active dropdown item
  $('#down-arrow').appendTo('.tb-megamenu-item.level-1.mega.dropdown');
  $('.tb-megamenu-item.level-1.mega.dropdown').hover(
    function(){
      $(this).find('#down-arrow').show();
    }, function() {
      $(this).find('#down-arrow').hide();
  });
  // Add or remove this function to convert from mega menu to superfish
  // check the browser width and adjust the megamenu to the screen width
  function checkWidth() {
    var w = $(window).width() - 30;
    $('.tb-megamenu ul.level-0 li.tb-megamenu-item.level-1 .tb-megamenu-submenu .mega-dropdown-inner').css('width', w).css('max-width', '1160px');
  }
  // Execute on load
  checkWidth();
  // Bind event listener
  $(window).resize(checkWidth);
});
