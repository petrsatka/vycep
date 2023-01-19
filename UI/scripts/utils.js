"use strict";

/*
  Sdružuje výpisy notifikací a chyb.
*/
let notify = {};
/*
  Vypíše chybu do message boxu.
*/
notify.isError = false;
notify.error = function(message) {
  if (!notify.isError) {
    notify.isError = true;
    notify.alert('Chyba!', message, () => {
      notify.isError = false;  
    });
  }
},

notify.alert = function(title, message, callback) {
  $.alert({
    title: title,
    content: message,
    container:'.page-wrapper',
    boxWidth: '90%',
    useBootstrap: false,
    animateFromElement:false,
    buttons: {
      ok: function () {
        if (callback) {
          callback();
        }
      }
    }
  });
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
let api = {};

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
api.getCurrentUserBillCount = function(callback) {
  setTimeout(() => callback(api.billCountDEBUG,"ok", null),1000);
  //setTimeout(() => callback(-1,"unable_to_get_bcount", null),500);
  //setTimeout(() => callback(null, null, "500 - bad request"),500);
}

/*
  Získá aktuální počet nápojů na účtě uživatele
*/
api.getUserBillCount = function(username, callback) {
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
api.pay = function(username, count, callback) {
  if (count > api.billCountDEBUG || count < 0) {
    setTimeout(() => callback({
      paid: 0,
      billCount: api.billCountDEBUG
    },"invalid-payment-value", null), 1000);
  } else { 
    let paid = Math.min(api.billCountDEBUG, Math.max(0, count));
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

/*DEBUG*/
api.settings = {
  'newUsersPayment' : true,
  'ssid': 'coolap',
  'skey': 'coolSecurityKey',
  'pcount': 345,
  'inactTimeout': 10,
  'underLimTimeout': 5,
  'mode': 'auto'
}

api.setSettingsValue = function(name, value, callback) {
  let val = api.settings[name];
  if (val === undefined) {
    setTimeout(() => callback(null, "unable_to_set_settings_value", null), 800); 
  } else {
    setTimeout(() => callback({name: name, value: value}, "ok", null), 800);  
  }
  //setTimeout(() => callback(null, null, "500 - bad request"),500);  
}

api.getSettingsValue = function(name, callback) {
  let val = api.settings[name];
  if (val === undefined) {
    setTimeout(() => callback(null, "unable_to_get_settings_value", null), 800); 
  } else {
    setTimeout(() => callback(val,"ok", null),800);
  }
  //setTimeout(() => callback(null, null, "500 - bad request"),500);
}

api.setPermissionValue = function(username, permissionKey, value, callback) {
  if (permissionKey == 'admin' || permissionKey == 'payment') {
    setTimeout(() => callback({username : username, permissionKey: permissionKey, value: value}, "ok", null), 800); 
  } else {
    setTimeout(() => callback(null, "unable_to_set_permissions_value", null), 800);
  }
  //setTimeout(() => callback(null, null, "500 - bad request"),500); 
}

api.getIP = function(callback) {
  setTimeout(() => callback('192.168.1.45',"ok", null),800);  
}

api.getGateway = function(callback) {
  setTimeout(() => callback('192.168.1.1',"ok", null),800);  
}

api.resetPassword = function(username, callback) {
  setTimeout(() => callback('xyz7abc',"ok", null),1000);
  //setTimeout(() => callback(null, "unable_to_reset_password", null),500);
  //setTimeout(() => callback(null, null, "500 - bad request"),500);
}

api.deleteUser = function(username, callback) {
  setTimeout(() => callback(username,"ok", null),1000);
  //setTimeout(() => callback(null, "unable_to_delete_user", null),500);
  //setTimeout(() => callback(null, null, "500 - bad request"),500);
}

api.calibrate = function(callback) {
  api.settings['pcount'] = 330;
  setTimeout(() => callback(true,"ok", null),1000);
  //setTimeout(() => callback(false, "unable_to_get_users", null),500);
  //setTimeout(() => callback(null, null, "500 - bad request"),500);
}

api.stopCalibration = function(callback) {
  setTimeout(() => callback(true,"ok", null),1000);
  //setTimeout(() => callback(false, "unable_to_get_users", null),500);
  //setTimeout(() => callback(null, null, "500 - bad request"),500);
}


/*
Správa cookies
*/
let cookies = {};

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
let gui = {};

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
  let regEx = /^[0-9a-zA-Z]+$/;
  let minLen = 5;
  let maxLen = 15;
  return gui.validate(username, "Není vyplněno jméno.") && 
  gui.validate(username.length >= minLen, `Minimální délka jména je ${minLen} znaků.`) &&
  gui.validate(username.length <= maxLen, `Maximální délka jména je ${maxLen} znaků.`) && 
  gui.validate(username.match(regEx), "Jméno smí obsahovat pouze písmena a číslice.");
}

/*
  Validace hesla
*/
gui.validatePassword = function(password) {
  let minLen = 6;
  let maxLen = 30;
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
  let message = '';; 
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
  api.createAdmin(username, password, (result, resultCode, errorMessage) => {
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
  api.createUser(username, password, (result, resultCode, errorMessage) => {
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
  api.changePassword(oldPassword, password, (result, resultCode, errorMessage) => {
    gui.setInProgress(false);
    if (gui.handleError(resultCode, errorMessage)) {
      gui.validate(result, "Heslo nelze změnit.", "Heslo bylo změněno");
    }
  });  
}
 
/*
  Přihlášení
*/
gui.login = function(username, password, navigationTarget) {
  gui.setInProgress(true);
  api.login(username, password, (result, resultCode, errorMessage) => {
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
  api.logout((result, resultCode, errorMessage) => {
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
  apiMethod((result, resultCode, errorMessage) => {
    let isError = true;
    if (gui.handleError(resultCode, errorMessage, true)) {
      isError = false;
      if ($(targetElementSelector).is(':input')) { 
        if ($(targetElementSelector).is(':checkbox')) {
          $(targetElementSelector).prop("checked", !!result);
        } else if ($(targetElementSelector).is(':radio')) {
          $(targetElementSelector).val([result]);
        } else {      
          $(targetElementSelector).val(result);
        }
      } else {
        $(targetElementSelector).text(result);
      }
      
      $(targetElementSelector).prop('disabled', false);
    } else {
      if ($(targetElementSelector).is(':input')) {
        if ($(targetElementSelector).is(':checkbox')) {
          $(targetElementSelector).prop("checked", false);
        } else if ($(targetElementSelector).is(':radio')) {
          $(targetElementSelector).prop("checked", false);
        } else {    
          $(targetElementSelector).val("");
        } 
      } else {
        $(targetElementSelector).text('-');
      }
    }
    
    if (callback) {
      callback(result, isError);
    }
  });
}

gui.loadSettingsValue = function(name, targetElementSelector, callback) {
  gui.loadValue(function(cb) {api.getSettingsValue(name, cb);}, targetElementSelector, callback);
}

gui.setSettingsValue = function(name, value) {
  api.setSettingsValue(name, value, (result, resultCode, errorMessage) => {
    gui.handleError(resultCode, errorMessage, true);
    console.log(`set ${name} ${value}`);  
  });
}

gui.setPermissionValue = function(username, permissionKey, value) {
  api.setPermissionValue(username, permissionKey, value, (result, resultCode, errorMessage) => {
    gui.handleError(resultCode, errorMessage, true);  
  });
}

/*
  Vytvoří objednávku a hdnoty fronty a počtu nápůjů na účtu nastaví do qCountTargetSelector, bCountTargetSelector
*/
gui.makeOrder = function(qCountTargetSelector, bCountTargetSelector) {
  notify.message();
  gui.setInProgress(true);
  api.makeOrder((result, resultCode, errorMessage) => {
    gui.setInProgress(false);
    if (gui.handleError(resultCode, errorMessage)) {
      if (qCountTargetSelector) {
        $(qCountTargetSelector).text(result.queueCount);
      }
      
      if (bCountTargetSelector) {
        $(bCountTargetSelector).text(result.billCount);
      }
      
      gui.validate(resultCode == 'ok', null, "Objednáno", 5000);
    }
  });
}

/*
  Provede platbu
*/
gui.pay = function(username, count, callback) {
  notify.message();
  gui.setInProgress(true);
  api.pay(username, count, (result, resultCode, errorMessage) => {
    let isOK = gui.handleError(resultCode, errorMessage);    
    if (callback) {
      callback(result, !isOK);
    }
    
    gui.setInProgress(false);
  });
}

gui.loadUsers = function(callback) {
  api.loadUsers((result, resultCode, errorMessage) => {
    if (gui.handleError(resultCode, errorMessage, true)) {
      if (callback) {
        callback(result);
      }
    }
  });
}

gui.deleteUser = function(username, callback) {
  api.deleteUser(username, (result, resultCode, errorMessage) => {
    let isError = true;
    if (gui.handleError(resultCode, errorMessage, true)) {
      isError = false;
    }
    
    if (callback) {
      callback(result, isError);
    }
  });
}

gui.resetPassword = function(username, callback) {
  api.resetPassword(username, (result, resultCode, errorMessage) => {
    let isError = true;
    if (gui.handleError(resultCode, errorMessage, true)) {
      isError = false;
    }
    
    if (callback) {
      callback(result, isError);
    }
  });
}

gui.onInputChange = function(targetSelector, callback) {
  $(targetSelector).change(function() {
    var val = null;
    if ($(this).is(':checkbox')) {
      val =  $(this).is(':checked'); 
    } else {
      val = $(this).val()
    }
    
    if (callback) {
      callback(this, val);
    }
  });
}

gui.calibrate = function(callback) {
  gui.setInProgress(true);
  api.calibrate((result, resultCode, errorMessage) => {
    if (gui.handleError(resultCode, errorMessage, true)) {
      if (callback) {
        callback(result);
      }
    }
    
    gui.setInProgress(false);
  });
}

gui.stopCalibration = function(callback) {
  api.stopCalibration((result, resultCode, errorMessage) => {
    if (gui.handleError(resultCode, errorMessage, true)) {
      if (callback) {
        callback(result);
      }
    }
  });
}

/*gui.setInputsEnabled = function(parentSelector, isEnabled) => {
  $(parentSelector + ' input').prop('disabled', !isEnabled);
}*/

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
let firstRegistration = {};
firstRegistration.createAdmin = function() {
  gui.createAdmin($('#name').val(), $('#password').val(), $('#password-verification').val(), 'settings.html');
}

let registration = {};
  registration.createUser = function() {
  gui.createUser($('#name').val(), $('#password').val(), $('#password-verification').val(), 'orders.html');
}

let login = {};
login.login = function() {
  gui.login($('#name').val(), $('#password').val(), 'orders.html');
}

login.logout = function() {
  gui.logout('login.html');
}

let passwordChange = {};
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

let orders = {};
orders.loadQCount = function() {
  gui.loadValue(api.getQueueCount, "#q-count");
}

orders.loadBCount = function() {
  gui.loadValue(api.getCurrentUserBillCount, "#b-count");
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

let payment = {};
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
  gui.pay(payment.getUsername(), $("#count").val(), (result, isError) => {
    if (!isError) {
      payment.setCountValue(result.billCount);
      payment.setPaidMessage(result.paid);
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
  gui.loadValue((callback) => {api.getUserBillCount(payment.getUsername(), callback)}, "#count", (count, isError) => {
    if (!isError) {
      gui.setInProgress(false);
    }
  });
}

payment.getUsername = function() {
  return $(location).attr('hash').replace("#", '') || cookies.getUsername();
}                                          

payment.loadValues = function() {
  $("#username").text(payment.getUsername());
  payment.loadBCount();
}

payment.onPageShow = function() {
  payment.loadValues();
}

let users = {}
users.generateCheckboxCell = function(item, propertyName) {
  return `<td> <input ${item[propertyName+'CheckboxEnabled'] ? '' : 'disabled'} type="checkbox" onchange="users.handleCheckboxChange('${item.username}', '${propertyName}', event)" ${item[propertyName] ? ' checked' : ''}> </td>`;
}

users.generateButtonCell = function(item, value, action) {  
  return `<td> <input ${item[action+'ButtonEnabled'] ? '' : 'disabled'} type="button" value="${value}" onclick="users.handleButtonClick('${item.username}','${action}', event)"> </td>`;
}

users.generateRow = function(item) {
    let row = `<tr class="user-row-${item.username}"> <td> ${item.username} </td> `;
    row += users.generateCheckboxCell(item, 'payment');
    row += users.generateCheckboxCell(item, 'admin');
    row += `</tr> <tr class="user-row-${item.username}">`;
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

users.handleCheckboxChange = function(username, permissionKey, event) {
  let checkbox = event.target;
  let status = checkbox.checked;
  gui.setPermissionValue(username, permissionKey, status);
}

users.handleButtonClick = function(username, action, event) {
  $(event.target).prop("disabled", true);
  let enableButton = () => {
    $(event.target).prop("disabled", false);
  };
  switch(action) {
    case 'passwordReset':
      users.resetPassword(username, enableButton);
    break;
    case 'pay':
      enableButton();
      users.pay(username);
    break
    case 'delete':
      users.deleteUser(username, enableButton);
    break;
  }
}

users.pay = function(username) {
  gui.navigate('payment.html#'+ username)
}

users.loadUsers = function() {
  gui.loadUsers((tableRowsData) => {
   $('#users-table-holder').html(users.createTable(tableRowsData));
  });  
}

users.loadAllowPayment = function() {
  gui.loadSettingsValue('newUsersPayment', '#allow-payment');
}

users.onAllowPaymentCheckboxChange = function(event) {
  let checkbox = event.target;
  let status = checkbox.checked;
  gui.setSettingsValue('newUsersPayment', status); 
}

users.resetPassword = function(username, callback) {
  gui.resetPassword(username, (result, isError) => {
    if (!isError) {
      notify.alert('Nové heslo', result);
    }
    
    if (callback) {
      callback(result, isError);
    }  
  });
}

users.deleteUser = function(username, callback) {
  gui.deleteUser(username, (result, isError) => {
    if(!isError) {
      $('.user-row-' + username).remove();
    }
    
    if (callback) {
      callback(result, isError);
    }   
  });
}

users.onPageShow = function() {
  users.loadUsers();
  users.loadAllowPayment();
}

let settings = {};

settings.loadNetworkInfo = function() {
  gui.loadValue(api.getGateway, '#gateway');
  gui.loadValue(api.getIP, '#ip');
}

settings.registerChange = function() {
  gui.onInputChange('#ssid', function(element, val) {gui.setSettingsValue('ssid',val);});
  gui.onInputChange('#skey', function(element, val) {gui.setSettingsValue('skey',val);});
  gui.onInputChange('#pcount', function(element, val) {gui.setSettingsValue('pcount',val);});
  gui.onInputChange('[name="mode"]', function(element, val) {gui.setSettingsValue('mode',val);});
  gui.onInputChange('#inactivity-timeout', function(element, val) {gui.setSettingsValue('inactTimeout',val);});
  gui.onInputChange('#under-limit-timeout', function(element, val) {gui.setSettingsValue('underLimTimeout',val);});
}

settings.loadValues = function() {
  gui.loadSettingsValue('ssid', '#ssid');
  gui.loadSettingsValue('skey', '#skey');
  gui.loadSettingsValue('pcount', '#pcount');
  gui.loadSettingsValue('mode', '[name="mode"]');
  gui.loadSettingsValue('inactTimeout', '#inactivity-timeout');
  gui.loadSettingsValue('underLimTimeout', '#nder-limit-timeout');
  settings.loadNetworkInfo();
}

settings.onPageShow = function() {
  gui.stopCalibration();
  settings.registerChange();
  settings.loadValues();
}

settings.calibrate = function() {
  gui.calibrate(() => {
    $("#pcount").prop('disabled', true);
    $("#pcount").val("");
    $("#button-calibrate").addClass("not-displayed-temporary");
    $("#button-done").addClass("displayed-temporary");  
  });
}

settings.done = function() {
  gui.setInProgress(true);
  gui.stopCalibration(() => {   
    gui.loadSettingsValue('pcount', '#pcount', () => {
      $("#button-calibrate").removeClass("not-displayed-temporary");
      $("#button-done").removeClass("displayed-temporary");
      gui.setInProgress(false); 
    });
  }); 
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