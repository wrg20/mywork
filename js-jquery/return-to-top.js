// ===== Scroll to Top ====
$(window).scroll(function() {
    if ($(this).scrollTop() >= 50) {        // If page is scrolled more than 50px
        $('#return-to-top').fadeIn(200);    // Fade in the arrow
    } else {
        $('#return-to-top').fadeOut(200);   // Else fade out the arrow
    }
});
// When arrow is clicked scroll to the top of the body
function returnToTop() {
  $('body,html').animate({
    scrollTop : 0
  }, 500);
}
function keyHandler(event) {
  switch (event.which) {
    case KEY_SPACE: {
      event.stopPropagation;
      return returnToTop();
      break;
    }
  } //end switch
  return true;
}
