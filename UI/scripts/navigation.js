import { menuContent } from './menu-content.js';

(function createNavigation() {
  var path = window.location.pathname;
  var page = path.split("/").pop();
  page = page.substring(0, page.indexOf(".html") + 5);
  var menuItemString = menuContent.filter(itm => itm.link.indexOf(page) < 0).map(itm => `<a href="${itm.link}">${itm.title}</a>`).join('').trim();
  if (!menuItemString) {
    return;
  }
  
  $(".main-header").append(`
  <nav id="hamnav">
    <label for="hamburger">&#9776;</label>
    <input type="checkbox" id="hamburger"/>
    <div id="hamitems">
      ${menuItemString}
    </div>
  </nav>
  `);
}());
