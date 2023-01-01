import { menuContent } from './menu-content.js';

(function createNavigation() {
  var menuItemString = menuContent.map(itm => `<a href="${itm.link}">${itm.title}</a>`).join('');
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
