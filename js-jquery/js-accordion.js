//pull the id of the accordion in case there are more than one accordion on a page
var id = $('div.c-accordion').attr("id");
$('#'+id+' a.c-accordion__a').click(function(event){
  //event.defaultPrevented; jQuery 2.x
  event.preventDefault();
  $(this).parent().toggleClass('c-accordion__li--active');
  $(this).toggleClass('c-accordion__a--active');
  $(this).next("div.c-accordion__content").slideToggle().toggleClass('c-accordion__content--active');
});
