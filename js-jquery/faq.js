$(document).ready(function() {
  $('.faq-section').click(function(){
    $(this).nextUntil('dl.accordion').slideToggle('slow');
    $(this).parent('dl').toggleClass('active');
    $(this).toggleClass('faq-section-arrow-down').toggleClass('faq-section-arrow-up');
  });
  $('dd.accordion-navigation').click(function(){
    $(this).find('.content').slideToggle();
    $(this).find('a').toggleClass('faq-arrow-down').toggleClass('faq-arrow-right');
  });
});
