/*
  Sdružuje výpisy notifikací a chyb.
*/
var notify = {};
/*
  Vypíše chybu do message boxu.
*/
notify.isError = false;
notify.error = function(message) {
  if (!notify.isError) {
    notify.isError = true;
    $.alert({
      title: 'Chyba!',
      content: message,
      container:'.page-wrapper',
      boxWidth: '90%',
      useBootstrap: false,
      buttons: {
        ok: function () {
          notify.isError = false;
        }
      }
    });
  }
},
  
notify.message_timeout_id = null;
/*
  Vypíše zprávu do připraveného místa na stránce.
*/
notify.message = function(message, error = false, timeout = 0) {
  if (notify.message_timeout_id) {
    clearTimeout(notify.message_timeout_id);
  }
    
  $('.message-holder').empty();
  if (message) {
    $('.message-holder').append(`<span ${error ? 'class="error-message"' : ""}>${message}</span>`);
  }
    
  if (timeout) {
    notify.message_timeout_id = setTimeout(() => {
        $('.message-holder').empty();
      },
      timeout
    );
  }
}

/*
  Volání zařízení - nejnižší vrstva
*/
var api = {};

/*
  Vytvoření administrátorského účtu.
*/
api.createAdmin = function(username, password, callback) {
  //DEBUG
  //Validace délek na serveru? - alespoň username
  //Odladit speciállní znaky v hesle
  //Omezit volání pouze při inicializaci
  //Test existujícího jména
  //Test chyby HTTP
  //Test neplatné návratové hodnty
  //setTimeout(() => callback(null, null, "500 - bad request"), 500);
  //setTimeout(() => callback(username,"user_exists", null),500);
  setTimeout(() => callback(username,"ok", null),500);
}

/*
  Vytvoření běžného účtu.
*/
api.createUser = function(username, password, callback) {
  //DEBUG Omezit volání pouze adminovi
  //setTimeout(() => callback(username,"user_exists", null), 500);
  //setTimeout(() => callback(null, null, "500 - bad request"), 500);
  //setTimeout(() => callback(username,"unknown_error", null), 500);
  if (username.toLowerCase() == 'franta') {
     setTimeout(() => callback(username,"user_exists", null),500);
  } else {
    setTimeout(() => callback(username,"ok", null),500);
  }
}

/*
  Změna hesla
*/
api.changePassword = function(oldPassword, password, callback) {
  if (oldPassword == '') {
    setTimeout(() => callback(false,"invalid_password", null),500);
  } else {
    //setTimeout(() => callback(null, null, "500 - bad request"),500);
    setTimeout(() => callback(true,"ok", null),500); 
    //setTimeout(() => callback(false,"ok", null),500);
  }  
}

api.resetPassword = function(username, callback) {

} 

api.deleteUser = function(username, callback) {

}

/*
  Přihlášení
*/
api.login = function(username, password, callback) {
  if (!username || !password) {
    setTimeout(() => callback(username,"bad_username_or_password", null),500);
  } else {
    setTimeout(() => callback(username,"ok", null),500);
  }
}

api.qCountDEBUG = 5;
api.billCountDEBUG = 20;
/*
  Získá aktuální počet nápojů ve frontě zařízení
*/
api.getQueueCount = function(callback) {
  setTimeout(() => callback(api.qCountDEBUG,"ok", null),500);
  //setTimeout(() => callback(-1,"unable_to_get_qcount", null), 500);
  //setTimeout(() => callback(null, null, "500 - bad request"), 500); 
}

/*
  Získá aktuální počet nápojů na účtě přihlášeného uživatele
*/
api.getUserBillCount = function(callback) {
  setTimeout(() => callback(api.billCountDEBUG,"ok", null),1000);
  //setTimeout(() => callback(-1,"unable_to_get_bcount", null),500);
  //setTimeout(() => callback(null, null, "500 - bad request"),500);
}

/*
  Vytvoří objednávků a vrátí aktuální počet nápojů ve frontě a na účtě přihlášeného uživatele
*/
api.makeOrder = function(callback) {
  setTimeout(() => callback({
   queueCount: ++api.qCountDEBUG,
   billCount: ++api.billCountDEBUG, 
  },"ok", null), 1000);

  /*setTimeout(() => callback({
    queueCount: api.qCountDEBUG,
    billCount: api.billCountDEBUG, 
  },"unable_to_make_order", null),5000); */
  //setTimeout(() => callback(null, null, "500 - bad request"), 500); 
}
  
/*
  Provede platbu a vrátí zaplacený počet a počet nápojů na účtě přihlášeného uživatele
*/
api.pay = function(count, callback) {
  if (count > api.billCountDEBUG || count < 0) {
    setTimeout(() => callback({
      paid: 0,
      billCount: api.billCountDEBUG
    },"invalid-payment-value", null), 1000);
  } else { 
    var paid = Math.min(api.billCountDEBUG, Math.max(0, count));
    api.billCountDEBUG -= paid;
    setTimeout(() => callback({
      paid: paid,
      billCount: api.billCountDEBUG
    },"ok", null), 1000);
  }   
  /*setTimeout(() => callback({
     paid: 0,
     billCount: api.billCountDEBUG,
  },"unable_to_make_payment", null), 1000);*/
  //setTimeout(() => callback(null, null, "500 - bad request"), 500); 
}

api.loadUsers = function(callback) {
  const jsonData = [
    { "username": "VLS", "payment": true, "admin": true, "paymentCheckboxEnabled":true, "adminCheckboxEnabled":true, "passwordResetButtonEnabled":true, "payButtonEnabled":true, "deleteButtonEnabled":true },
    { "username": "Pepa", "payment": false, "admin": false, "paymentCheckboxEnabled":true, "adminCheckboxEnabled":true, "passwordResetButtonEnabled":true, "payButtonEnabled":true, "deleteButtonEnabled":true },
    { "username": "Franta", "payment": true, "admin": true, "paymentCheckboxEnabled":false, "adminCheckboxEnabled":false, "passwordResetButtonEnabled":true, "payButtonEnabled":true, "deleteButtonEnabled":false },
    { "username": "Honza", "payment": true, "admin": false, "paymentCheckboxEnabled":true, "adminCheckboxEnabled":true, "passwordResetButtonEnabled":true, "payButtonEnabled":true, "deleteButtonEnabled":true },
    { "username": "Tonda", "payment": true, "admin": false, "paymentCheckboxEnabled":true, "adminCheckboxEnabled":true, "passwordResetButtonEnabled":true, "payButtonEnabled":true, "deleteButtonEnabled":true },
    { "username": "Petr", "payment": false, "admin": false, "paymentCheckboxEnabled":true, "adminCheckboxEnabled":true, "passwordResetButtonEnabled":true, "payButtonEnabled":true, "deleteButtonEnabled":true },
    { "username": "Pavel", "payment": false, "admin": false, "paymentCheckboxEnabled":true, "adminCheckboxEnabled":true, "passwordResetButtonEnabled":true, "payButtonEnabled":true, "deleteButtonEnabled":true },
  ]
  setTimeout(() => callback(jsonData,"ok", null),1000);
  //setTimeout(() => callback(null, "unable_to_get_users", null),500);
  //setTimeout(() => callback(null, null, "500 - bad request"),500);
}


/*
  Odhlášení
*/
api.logout = function(callback) {
  setTimeout(() => callback(true,"ok", null), 500);
}

api.setValue = function(name, value, callback) {

}

api.getValue = function(name, value, callback) {

}

/*
Správa cookies
*/
var cookies = {};

/*
Vrátí jmnéno aktuálně přihlášeného uživatele
*/
cookies.getUsername = function() {
  return "Franta"; 
}

/*
  Vrstva nad voláním api
  Řeší validaci vstupů a obsluhu chyb
*/
var gui = {};

/*
  Přechod na jinou stránku
*/
gui.navigate = function(target) {
  window.location.href = target;
}

/*
  Vrací předchozí stránku, pokud neeexistuje vracíme se do objednávek
*/
gui.getReferrer = function() {
  return document.referrer || 'orders.html';
}

/*
  Naviguje na předchozí stránku
*/
gui.navigateToReferrer = function() {
  gui.navigate(gui.getReferrer());
}

/*
  Výpis validační, nebo konfirmační hlášky podle podmínky isOK
*/
gui.validate = function(isOK, errorMessage, okMessage = "", timeout = 0){
  notify.message();
  if (isOK && okMessage) {
    notify.message(okMessage, false, timeout);
  } else if(!isOK && errorMessage) {
    notify.message(errorMessage, true, timeout);
  }
  
  return isOK;
}

/*
  Validace uživatelského jména
*/
gui.validateUsername = function(username) {
  var regEx = /^[0-9a-zA-Z]+$/;
  var minLen = 5;
  var maxLen = 15;
  return gui.validate(username, "Není vyplněno jméno.") && 
  gui.validate(username.length >= minLen, `Minimální délka jména je ${minLen} znaků.`) &&
  gui.validate(username.length <= maxLen, `Maximální délka jména je ${maxLen} znaků.`) && 
  gui.validate(username.match(regEx), "Jméno smí obsahovat pouze písmena a číslice.");
}

/*
  Validace hesla
*/
gui.validatePassword = function(password) {
  var minLen = 6;
  var maxLen = 30;
  return gui.validate(password, "Není vyplněno heslo.") && 
  gui.validate(password.length >= minLen, `Minimální délka hesla je ${minLen} znaků.`) &&
  gui.validate(password.length <= maxLen, `Maximální délka hesla je ${maxLen} znaků.`);
}

/*
  Verifikace hesla
*/
gui.verifyPassword = function(password, passwordVerification) {
  return gui.validate(password == passwordVerification, "Neshoduje se ověření hesla.");
}

/*
  Obsluha chybných odpovědí ze serveru
*/
gui.handleError = function(resultCode, errorMessage, popupWindow = false) {
  if (errorMessage) {
    //http chyby
    notify.error(errorMessage);
    return false;
  }
  
  if (resultCode == 'ok') {
      return true;
  }
  
  //vlastní chyby
  var message = '';; 
  switch(resultCode) {
    case 'user_exists':
      message = 'Uživatel s tímto jménem už existuje.';
      break;
    case 'bad_username_or_password':
      message = 'Neplatné jméno nebo heslo.';
      break;
    case 'invalid_password':
      message = 'Neplatné heslo';
      break
    case 'invalid-payment-value':
      message = 'Nelze zaplatit neplatné množství.';
      break;
  }
  
  if (message) {
    if (popupWindow) {
      notify.error(message);
    } else {
      notify.message(message, true);
    }
  } else {
    notify.error(resultCode);
  }
  
  return false;
}

gui.setInProgress = function(inProgress) {
   $(':button.button-large').prop("disabled", inProgress);
   return true;  
}

/*
  Vytvoření administrátorského účtu
*/
gui.createAdmin = function(username, password, passwordVerification, navigationTarget) {
  gui.validateUsername(username) &&
  gui.validatePassword(password) &&
  gui.verifyPassword(password, passwordVerification) &&
  gui.setInProgress(true) &&
  api.createAdmin(username, password, (value, resultCode, errorMessage) => {
    gui.setInProgress(false);
    if (gui.handleError(resultCode, errorMessage)) {
      gui.navigate(navigationTarget);
    }  
  });
}

/*
  Vytvoření běžného účtu
*/
gui.createUser = function(username, password, passwordVerification, navigationTarget) {
  gui.validateUsername(username) &&
  gui.validatePassword(password) &&
  gui.verifyPassword(password, passwordVerification) &&
  gui.setInProgress(true) &&
  api.createUser(username, password, (value, resultCode, errorMessage) => {
    gui.setInProgress(false);
    if (gui.handleError(resultCode, errorMessage)) {
      gui.navigate(navigationTarget);
    }  
  });
}

 /*
  Změna hesla
 */
gui.changePassword = function(oldPassword, password, passwordVerification) {
  gui.validatePassword(password) &&
  gui.verifyPassword(password, passwordVerification) &&
  gui.setInProgress(true) &&
  api.changePassword(oldPassword, password, (value, resultCode, errorMessage) => {
    gui.setInProgress(false);
    if (gui.handleError(resultCode, errorMessage)) {
      gui.validate(value, "Heslo nelze změnit.", "Heslo bylo změněno");
    }
  });  
}
 
/*
  Přihlášení
*/
gui.login = function(username, password, navigationTarget) {
  gui.setInProgress(true);
  api.login(username, password, (value, resultCode, errorMessage) => {
    gui.setInProgress(false);
    if (gui.handleError(resultCode, errorMessage)) {
      gui.navigate(navigationTarget);
    }  
  });
}

/*
  Odhlášení
*/
gui.logout = function(navigationTarget) {
  gui.setInProgress(true);
  api.logout((value, resultCode, errorMessage) => {
    gui.setInProgress(false);
    if (gui.handleError(resultCode, errorMessage, true)) {
      gui.navigate(navigationTarget);
    }  
  });
}

/*
  Načtení hodnoty pomocí apiMethod a vložení do targetElementSelector
*/
gui.loadValue = function(apiMethod, targetElementSelector, callback) {
  $(targetElementSelector).prop('disabled', true);
  apiMethod((value, resultCode, errorMessage) => {
    var isError = true;
    if (gui.handleError(resultCode, errorMessage, true)) {
      isError = false;
      if ($(targetElementSelector).is(':input')) { 
        $(targetElementSelector).val(value);
      } else {
        $(targetElementSelector).text(value);
      }
    } else {
      if ($(targetElementSelector).is(':input')) { 
         $(targetElementSelector).val(""); 
      } else {
        $(targetElementSelector).text('-');
      }
    }
    
    if (callback) {
      callback(value, isError);
    }
    
    $(targetElementSelector).prop('disabled', false);
  });
}

/*
  Vytvoří objednávku a hdnoty fronty a počtu nápůjů na účtu nastaví do qCountTargetSelector, bCountTargetSelector
*/
gui.makeOrder = function(qCountTargetSelector, bCountTargetSelector) {
  notify.message();
  gui.setInProgress(true);
  api.makeOrder((value, resultCode, errorMessage) => {
    gui.setInProgress(false);
    if (gui.handleError(resultCode, errorMessage)) {
      if (qCountTargetSelector) {
        $(qCountTargetSelector).text(value.queueCount);
      }
      
      if (bCountTargetSelector) {
        $(bCountTargetSelector).text(value.billCount);
      }
      
      gui.validate(resultCode == 'ok', null, "Objednáno", 5000);
    }
  });
}

/*
  Provede platbu
*/
gui.pay = function(count, callback) {
  notify.message();
  gui.setInProgress(true);
  api.pay(count, (value, resultCode, errorMessage) => {
    var isOK = gui.handleError(resultCode, errorMessage);    
    if (callback) {
      callback(value, !isOK);
    }
    
    gui.setInProgress(false);
  });
}

gui.loadUsers = function(callback) {
  api.loadUsers((value, resultCode, errorMessage) => {
    if (gui.handleError(resultCode, errorMessage, true)) {
      if (callback) {
        callback(value);
      }
    }
  });
}

/*
  Maže citlivé položky
*/
gui.clearSensitiveInputs = function() {
  $('input#old-password').val("");
  $('input#name').val("");
  $('input#password').val("");
  $('input#password-verification').val("");
}

gui.removeTemporaryClasses = function() {
  $(".not-displayed-temporary").removeClass("not-displayed-temporary");
  $(".displayed-temporary").removeClass("displayed-temporary");  
}

gui.resetDefaultValues = function() {
  $("[data-defval]").each(function() {
    $(this).val($(this).data('defval'));
  });
}

///////////Metody jednotlivých stránek. Získají vstupní hodnoty a volají metody gui//////////////////////////
var firstRegistration = {};
firstRegistration.createAdmin = function() {
  gui.createAdmin($('#name').val(), $('#password').val(), $('#password-verification').val(), 'settings.html');
}

var registration = {};
  registration.createUser = function() {
  gui.createUser($('#name').val(), $('#password').val(), $('#password-verification').val(), 'orders.html');
}

var login = {};
login.login = function() {
  gui.login($('#name').val(), $('#password').val(), 'orders.html');
}

login.logout = function() {
  gui.logout('login.html');
}

var passwordChange = {};
passwordChange.clearValues = function() {
   gui.clearSensitiveInputs();
}

passwordChange.changePassword = function() {
   gui.changePassword($('#old-password').val(), $('#password').val(), $('#password-verification').val());
   passwordChange.clearValues();  
}

passwordChange.cancel = function() {
  passwordChange.clearValues();
  gui.navigateToReferrer();
}

var orders = {};
orders.loadQCount = function() {
  gui.loadValue(api.getQueueCount, "#q-count");
}

orders.loadBCount = function() {
  gui.loadValue(api.getUserBillCount, "#b-count");
}

orders.loadValues = function() {
  orders.loadQCount();
  orders.loadBCount();
}

orders.makeOrder = function() {
  gui.makeOrder("#q-count", "#b-count");
}

orders.navigateToPayment = function() {
  gui.navigate("payment.html");
}

orders.onPageShow = function() {
  orders.loadValues();
}

var payment = {};
payment.setCountValue = function(count) {
  $("#count").val(count || 0);  
}

payment.setPaidText = function(paid) {
  $("#paid").text(paid)
}

payment.setPaidMessage = function(paid) {
  payment.setPaidText(paid);
  $("#payment-form").addClass("not-displayed-temporary");
  $("#pay-button").addClass("not-displayed-temporary");
  $("#back-button").val("OK");
  $("#confirmation-message").addClass("displayed-temporary");
}

payment.pay = function() {
  $("#count").prop('disabled', true);
  gui.pay($("#count").val(), (value, isError) => {
    if (!isError) {
      payment.setCountValue(value.billCount);
      payment.setPaidMessage(value.paid);
    }
    
    $("#count").prop('disabled', false);
  });
}

payment.goBack = function() {
  passwordChange.clearValues();
  gui.navigateToReferrer();
}

payment.loadBCount = function() {
  gui.setInProgress(true);
  gui.loadValue(api.getUserBillCount, "#count", (count, isError) => {
    if (!isError) {
      gui.setInProgress(false);
    }
  });
}

payment.loadValues = function() {
  $("#username").text(cookies.getUsername());
  payment.loadBCount();
}

payment.onPageShow = function() {
  payment.loadValues();
}

var users = {}
users.generateCheckboxCell = function(item, propertyName) {
  return `<td> <input ${item[propertyName+'CheckboxEnabled'] ? '' : 'disabled'} type="checkbox" onchange="users.handleCheckboxChange('${item.username}', '${propertyName}', event)" ${item[propertyName] ? ' checked' : ''}> </td>`;
}

users.generateButtonCell = function(item, value, action) {  
  return `<td> <input ${item[action+'ButtonEnabled'] ? '' : 'disabled'} type="button" value="${value}" onclick="users.handleButtonClick('${item.username}','${action}')"> </td>`;
}

users.generateRow = function(item) {
    let row = `<tr> <td> ${item.username} </td> `;
    row += users.generateCheckboxCell(item, 'payment');
    row += users.generateCheckboxCell(item, 'admin');
    row += '</tr> <tr>';
    row += users.generateButtonCell(item, 'Reset hesla', 'passwordReset');
    row += users.generateButtonCell(item, 'Zaplatit', 'pay');
    row += users.generateButtonCell(item, 'Vymazat', 'delete');
    row += '</tr>';
    return row;
}

users.createTable = function(tableRowsData) {
  let html = '<table> <thead> <tr> <th>Jméno</th> <th>Platba</th> <th>Admin</th> </tr> </thead> <tbody>';

  tableRowsData.forEach(function(item) {
    html += users.generateRow(item);
  });

  html += '</tbody> </table>';
  return html;
}

users.handleCheckboxChange = function(name, propertyName, event) {
  let checkbox = event.target;
  let status = checkbox.checked;
  //call your method here with name and status as parameter
  console.log(name + ' ' + propertyName + ' ' + status);
}

users.handleButtonClick = function(name, action) {
   //call your method here with name and action as parameter
   console.log(name + ' ' + action);
}

users.loadUsers = function() {
  gui.loadUsers((tableRowsData) => {
   $('#users-table-holder').html(users.createTable(tableRowsData));
  });  
}

users.onPageShow = function() {
  users.loadUsers();
}

////Global Events////
$(window).on("pageshow",function(){
  gui.clearSensitiveInputs();
  gui.resetDefaultValues();
  gui.removeTemporaryClasses();
  notify.message();
});

//DEBUG
//Přihlášeného uživatel vždy místo login page směrovat na orders - vyřešit asi rovnou na serveru?
//favicon
//RETEST všech operací stránek s ohledem na zamykání při delším trvání odpovědi