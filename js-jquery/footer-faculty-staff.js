// footer faculty and staff menu
$(document).ready(function(){
  $('.internalcontent1, .internalcontent2, .internalcontent3').hide();
  $('#internallabel3').click(function() {
    $(this).toggleClass('active');
    $('.internalcontent3').slideToggle();
    $('.internalcontent2').hide();
    $('.internalcontent1').hide();
    $('#internallabel2').removeClass('active');
    $('#internallabel1').removeClass('active');
  });
  $('#internallabel2').click(function() {
    $(this).toggleClass('active');
    $('.internalcontent2').slideToggle().addClass('active');
    $('.internalcontent3').hide();
    $('.internalcontent1').hide();
    $('#internallabel3').removeClass('active');
    $('#internallabel1').removeClass('active');
  });
  $('#internallabel1').click(function() {
    $(this).toggleClass('active');
    $('.internalcontent3').hide();
    $('.internalcontent2').hide();
    $('.internalcontent1').slideToggle().addClass('active');
    $('#internallabel2').removeClass('active');
    $('#internallabel3').removeClass('active');
  });
});
