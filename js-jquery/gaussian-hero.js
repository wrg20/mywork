$(document).ready(function(){
  //get the image src so we can insert it as the background image later
  $('.node-hero-layer').each(function(){
    var getImageSrc = $(this).find('.node-hero-layer--panel__image').attr('src');
    // add div background image using the variable above
    $('.node-hero-layer--panel__header, .node-hero-layer--panel__footer, .node-hero-layer--panel__wrapper, .node-hero-layer--panel__content', this).css('background-image', 'url(' + getImageSrc + ')');
  });
  $('li.level-1 #gaussian-menu').appendTo('.dropdown-menu');
});
