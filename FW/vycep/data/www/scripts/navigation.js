function createNavigation(menu) {
  var path = window.location.pathname;
  var page = path.split("/").pop();
  page = page.substring(0, page.indexOf(".html") + 5);
  var menuItemString = menu.menuContent.filter(itm => itm.link.indexOf(page) < 0).map(itm => `<a href="${itm.link}">${itm.title}</a>`).join('').trim();  
  //Logout
  menuItemString += `<a href="#" onclick="login.logout();return false;">Odhlásit (${cookies.getUsername()})</a>`;

  $(".main-header").append(`
  <nav id="hamnav">
    <label for="hamburger">&#9776;</label>
    <input type="checkbox" id="hamburger" autocomplete="off"/>
    <div id="hamitems">
      ${menuItemString}
    </div>
  </nav>
  `);
};

import(`../rolespecific/menu-content.js?time=${Date.now()}`).then((menu) => {
  createNavigation(menu);
});

$(window).on("pageshow",function(){
  $("#hamburger").prop('checked', false)
});
