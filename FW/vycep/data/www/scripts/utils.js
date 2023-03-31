"use strict";

/*
  Sdružuje výpisy notifikací a chyb.
*/
let notify = {};
/*
  Vypíše chybu do okna.
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

/*
  Vypíše zprávu do okna
*/
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
api.parseResponseData = function (data) {
  if (data) {
    return data.split("&");
  }
  
  return [];
}

api.post = function(url, reqData, callback) {
  $.ajax({
    method: "POST",
    url: url,
    data:reqData,
    success: function(data, textStatus, xhr) {
        callback(data, null)
    },
    error: function(xhr, textStatus, errorText) {
         callback(null, errorText)
    }, 
    statusCode: {
      401: function() {
        api.logout(() => {
          gui.navigate("login.html");
        });
      }
    }
  });
}

/*
  Vytvoření administrátorského účtu.
*/
api.createFirstAdmin = function(username, password, callback) {
  api.post("/api/createFirstAdmin", {username: username, password: password}, (resData, errorText) => {
    var results = api.parseResponseData(resData, errorText);
    callback(null, results[0], errorText);
  });
}

/*
  Vytvoření běžného účtu.
*/
api.createUser = function(username, password, callback) {
  api.post("/api/createUser", {username: username, password: password}, (resData, errorText) => {
    var results = api.parseResponseData(resData, errorText);
    callback(null, results[0], errorText);
  });
}

/*
  Změna hesla
*/
api.changePassword = function(oldPassword, newPassword, callback) {
  api.post("/api/changePassword", {oldpassword: oldPassword, newpassword: newPassword}, (resData, errorText) => {
    var results = api.parseResponseData(resData, errorText);
    callback(null, results[0], errorText);
  });  
}

/*
  Přihlášení
*/
api.login = function(username, password, callback) {
   api.post("/api/login", {username: username, password: password}, (resData, errorText) => {
    var results = api.parseResponseData(resData, errorText);
    callback(null, results[0], errorText);
  });
}

api.qCountDEBUG = 5;
api.billCountDEBUG = 20;
/*
  Získá aktuální počet nápojů ve frontě zařízení
*/
api.getQueueCount = function(callback) {
  setTimeout(() => callback(api.qCountDEBUG,"OK", null), 1000); 
}

/*
  Získá aktuální počet nápojů na účtě přihlášeného uživatele
*/
api.getCurrentUserBillCount = function(callback) {
  api.post("/api/getCurrentUserBillCount", null, (resData, errorText) => {
    var results = api.parseResponseData(resData, errorText);
    callback(results[1], results[0], errorText);
  });
}

/*
  Získá aktuální počet nápojů na účtě uživatele
*/
api.getUserBillCount = function(username, callback) {
  api.post("/api/getUserBillCount", {username: username}, (resData, errorText) => {
    var results = api.parseResponseData(resData, errorText);
    callback(results[1], results[0], errorText);
  });
}

/*
  Vytvoří objednávků a vrátí aktuální počet nápojů ve frontě a na účtě přihlášeného uživatele
*/
api.makeOrder = function(callback) {
  setTimeout(() => callback({
   queueCount: ++api.qCountDEBUG,
   billCount: ++api.billCountDEBUG, 
  },"OK", null), 1000);
}
  
/*
  Provede platbu a vrátí zaplacený počet a počet nápojů na účtě zvoleného uživatele
*/
api.payForUser = function(username, count, callback) {
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
    },"OK", null), 1000);
  }   
}

api.pay = function( count, callback) {
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
    },"OK", null), 1000);
  }   
}

api.debugUsers = [
{ "username": "Franta", "payment": true, "admin": true, "active": true, "paymentCheckboxEnabled":false, "adminCheckboxEnabled":false, "passwordResetButtonEnabled":true, "payButtonEnabled":true, "deleteButtonEnabled":false, "activateButtonEnabled":false },
{ "username": "Honza", "payment": true, "admin": false, "active": true, "paymentCheckboxEnabled":true, "adminCheckboxEnabled":true, "passwordResetButtonEnabled":true, "payButtonEnabled":true, "deleteButtonEnabled":true, "activateButtonEnabled":false },
{ "username": "Pavel", "payment": false, "admin": false, "active": true, "paymentCheckboxEnabled":true, "adminCheckboxEnabled":true, "passwordResetButtonEnabled":true, "payButtonEnabled":true, "deleteButtonEnabled":true, "activateButtonEnabled":false },
{ "username": "Pepa", "payment": false, "admin": false, "active": true, "paymentCheckboxEnabled":true, "adminCheckboxEnabled":true, "passwordResetButtonEnabled":true, "payButtonEnabled":true, "deleteButtonEnabled":true, "activateButtonEnabled":false },
{ "username": "Petr", "payment": false, "admin": false, "active": true, "paymentCheckboxEnabled":true, "adminCheckboxEnabled":true, "passwordResetButtonEnabled":true, "payButtonEnabled":true, "deleteButtonEnabled":true, "activateButtonEnabled":false },
{ "username": "Tonda", "payment": true, "admin": false, "active": false, "paymentCheckboxEnabled":false, "adminCheckboxEnabled":false, "passwordResetButtonEnabled":false, "payButtonEnabled":false, "deleteButtonEnabled":true, "activateButtonEnabled":true },
{ "username": "VLS", "payment": true, "admin": true, "active": true, "paymentCheckboxEnabled":true, "adminCheckboxEnabled":true, "passwordResetButtonEnabled":true, "payButtonEnabled":true, "deleteButtonEnabled":true, "activateButtonEnabled":false }
]

api.parseGetUsersJSONData = function(jsonData) {
  var resp = JSON.parse(jsonData);
  return resp.users.map(usr => {
    return {
      ...usr,
      paymentCheckboxEnabled: cookies.getUsername() !== usr.username && usr.active,
      adminCheckboxEnabled: cookies.getUsername() !== usr.username && usr.active,
      passwordResetButtonEnabled: cookies.getUsername() !== usr.username,
      payButtonEnabled: resp.paymentEnabled && usr.active,
      deleteButtonEnabled: cookies.getUsername() !== usr.username,
      activateButtonEnabled:cookies.getUsername() !== usr.username && !usr.active,
    }
  });
}

/*
  Načte uživatele
*/
api.getUsers = function(callback) {
  //setTimeout(() => callback(api.debugUsers,"OK", null), 1000);
  api.post("/api/getUsers", null, (resData, errorText) => {
    var results = api.parseResponseData(resData, errorText);
    callback(api.parseGetUsersJSONData(results[1]), results[0], errorText);
  });
}


/*
  Odhlášení
*/
api.logout = function(callback) {
  //setTimeout(() => callback(true,"OK", null), 1000);
  api.post("/api/logout", null, (resData, errorText) => {
    var results = api.parseResponseData(resData, errorText);
    callback(null, results[0], errorText);
  });
}

/*
  Uloží hodnotu nastavení
*/
api.setSettingsValue = function(name, value, callback) {
   api.post("/api/setSettingsValue", {key: name, value: value}, (resData, errorText) => {
    var results = api.parseResponseData(resData, errorText);
    callback(null, results[0], errorText);
  });  
}

/*
  Získá hodnotu nastavení
*/
api.getSettingsValue = function(name, callback) {
   api.post("/api/getSettingsValue", {key: name}, (resData, errorText) => {
    var results = api.parseResponseData(resData, errorText);
    callback(results[1], results[0], errorText);
  });
}

/*
  Uloží hodnotu oprávnění
*/
api.setPermissionValue = function(username, permissionKey, value, callback) {
  api.post("/api/setPermissionsValue", {key: permissionKey, value:value}, (resData, errorText) => {
    var results = api.parseResponseData(resData, errorText);
    callback(null, results[0], errorText);
  });
  
  //if (permissionKey == 'admin' || permissionKey == 'payment') {
  //  setTimeout(() => callback({username : username, permissionKey: permissionKey, value: value}, "OK", null), 1000); 
  //} else {
  //  setTimeout(() => callback(null, "unable_to_set_permissions_value", null), 1000);
  //}
}

/*
  Získá IP adresu zařízení
*/
api.getIP = function(callback) {
  //setTimeout(() => callback('192.168.1.45',"OK", null), 1000);
  api.post("/api/getIP", null, (resData, errorText) => {
    var results = api.parseResponseData(resData, errorText);
    callback(results[1], results[0], errorText);
  });  
}

/*
  Získá MAC adresu zařízení
*/
api.getMAC = function(callback) {
  api.post("/api/getMAC", null, (resData, errorText) => {
    var results = api.parseResponseData(resData, errorText);
    callback(results[1], results[0], errorText);
  });  
}

/*
  Získá bránu zařízení
*/
api.getGatewayIP = function(callback) {
  //setTimeout(() => callback('192.168.1.1',"OK", null), 1000);
  api.post("/api/getGatewayIP", null, (resData, errorText) => {
    var results = api.parseResponseData(resData, errorText);
    callback(results[1], results[0], errorText);
  }); 
}

/*
  Resetuje heslo
*/
api.resetPassword = function(username, callback) {
  setTimeout(() => callback('xyz7abc',"OK", null), 1000);
  //setTimeout(() => callback(null, "unable_to_reset_password", null), 1000);
  //setTimeout(() => callback(null, null, "500 - bad request"), 1000);
}

/*
  Vymaže uživatele
*/
api.deleteUser = function(username, callback) {
  setTimeout(() => callback(username,"OK", null), 1000);
  //setTimeout(() => callback(null, "unable_to_delete_user", null), 1000);
  //setTimeout(() => callback(null, null, "500 - bad request"), 1000);
}

/*
  Aktivuje nového uživatele
*/
api.activateUser = function(username, callback) {
  let user = api.debugUsers.find(element => element['username'] == 'Tonda');
  if (user) {
    user["active"] = true;
    user["paymentCheckboxEnabled"] = true;
    user["adminCheckboxEnabled"] = true;
    user["passwordResetButtonEnabled"] = true;
    user["payButtonEnabled"] = true;
    user["deleteButtonEnabled"] = true;
    user["activateButtonEnabled"] =true
  }
  
  setTimeout(() => callback(username,"OK", null), 1000);
  //setTimeout(() => callback(null, "unable_to_activate_user", null), 1000);
  //setTimeout(() => callback(null, null, "500 - bad request"), 1000);
}

/*
  Spustí kalibraci
*/
api.startCalibration = function(callback) {
  //api.settings['pcount'] = 330;
  console.log("calibration start");
  setTimeout(() => callback(true,"OK", null), 1000);
  //setTimeout(() => callback(false, "unable_to_start_calibration", null), 1000);
  //setTimeout(() => callback(null, null, "500 - bad request"), 1000);
}

/*
  Ukončí kalibraci
*/
api.stopCalibration = function(callback) {
  console.log("calibration stop");
  setTimeout(() => callback(true,"OK", null), 1000);
  //setTimeout(() => callback(false, "unable_to_stop_calibration", null), 1000);
  //setTimeout(() => callback(null, null, "500 - bad request"), 1000);
}

/*
  Spustí test
*/
api.runTest = function(callback) {
  console.log("test");
  setTimeout(() => callback(true,"OK", null), 1000);
  //setTimeout(() => callback(false, "unable_to_run_test", null), 1000);
  //setTimeout(() => callback(null, null, "500 - bad request"), 1000);
}

/*
  Restartuje zařízení
*/
api.restart = function(callback) {
  api.post("/api/restart", null, (resData, errorText) => {
    var results = api.parseResponseData(resData, errorText);
    callback(null, results[0], errorText);
  }); 
}

/*
  Připojí zařízení k AP. V případě úspěchu uloží ssid a securityKey a restartuje zařízení.
*/
api.setWifiConnection = function(ssid, securityKey, callback) {
  api.post("/api/setWifiConnection", {ssid: ssid, securitykey: securityKey}, (resData, errorText) => {
    var results = api.parseResponseData(resData, errorText);
    callback(null, results[0], errorText);
  }); 
}


/*
Správa cookies
*/
let cookies = {};

cookies.getCookieByName = function getCookie(name) {
  let value = `; ${document.cookie}`;
  let parts = value.split(`; ${name}=`);
  if (parts.length === 2){
    return parts.pop().split(';').shift()
  }
  
  return null;
}

/*
Vrátí jmnéno aktuálně přihlášeného uživatele
*/
cookies.getUsername = function() {
  return cookies.getCookieByName('ESPUNAME') || ''; 
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
  
  if (resultCode == "OK") {
      return true;
  }
  //vlastní chyby
  let message = ''; 
  switch(resultCode) {
    case 'USERNAME_SHORT':
      message = 'Uživatelské jméno musí obsahovat alespoň 5 znaků.';
      break;  
    case 'USERNAME_LONG':
      message = 'Uživatelské jméno může obsahovat maximálně 15 znaků.';
      break;  
    case 'USERNAME_INVALID_CHARACTERS':
      message = 'Uživatelské jméno může obsahovat pouze číslice a písmena.';
      break;  
    case 'USERNAME_EMPTY':
      message = 'Uživatelské jméno musí být vyplněno.';
      break;  
    case 'PASSWORD_SHORT':
      message = 'Heslo musí obsahovat alespoň 6 znaků.';
      break;  
    case 'PASSWORD_LONG':
      message = 'Heslo může obsahovat maximálně 30 znaků.';
      break;  
    case 'PASSWORD_EMPTY':
      message = 'Heslo musí být vyplněno.';
      break;
    case 'USERNAME_EXISTS':
      message = 'Uživatel s tímto jménem už existuje.';
      break;
    case 'UNKNOWN_ERROR':
      message = 'Neznámá chyba.';
      break;  
    case 'ANY_USER_EXISTS':
      message = 'V systému už existuje alespoň jeden uživatel.';
      break;
    case 'INVALID_USERNAME_OR_PASSWORD':
      message = 'Neplatné jméno nebo heslo';
      break;
    case 'USERNAME_NOT_EXISTS':
      message = 'Neplatný uživatel';
      break;   
    case 'INVALID_PASSWORD':
      message = 'Neplatné heslo';
      break;
    case 'SSID_TOO_LONG':
      message = 'Maximální délka SSID je 32 bytů';
      break;
      case 'SKEY_TOO_LONG':
      message = 'Maximální délka hesla je 63 bytů';
      break 
      
    /*case 'bad_username_or_password':
      message = 'Neplatné jméno nebo heslo.';
      break;
    case 'invalid_password':
      message = 'Neplatné heslo';
      break
    case 'invalid-payment-value':
      message = 'Nelze zaplatit neplatné množství.';
    case 'unnable_to_connect':
      message = 'Připojení se nezdařilo';
      break;  */
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
   $(':button.button-large:not(.always-enabled)').prop("disabled", inProgress);
   return true;  
}

/*
  Vytvoření administrátorského účtu
*/
gui.createFirstAdmin = function(username, password, passwordVerification, navigationTarget) {
  //gui.validateUsername(username) &&
  //gui.validatePassword(password) &&
  gui.verifyPassword(password, passwordVerification) &&
  gui.setInProgress(true) &&
  api.createFirstAdmin(username, password, (result, resultCode, errorMessage) => {
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
  //gui.validateUsername(username) &&
  //gui.validatePassword(password) &&
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
  //gui.validatePassword(password) &&
  gui.verifyPassword(password, passwordVerification) &&
  gui.setInProgress(true) &&
  api.changePassword(oldPassword, password, (result, resultCode, errorMessage) => {
    gui.setInProgress(false);
    if (gui.handleError(resultCode, errorMessage)) {
      //gui.validate(result, "Heslo nelze změnit.", "Heslo bylo změněno");
      notify.message("Heslo bylo změněno", false);
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
          $(targetElementSelector).prop("checked", result == 'true');
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

/*
  Načtení hodnoty nastavení
*/
gui.loadSettingsValue = function(name, targetElementSelector, callback) {
  gui.loadValue(function(cb) {api.getSettingsValue(name, cb);}, targetElementSelector, callback);
}

/*
  Uložení hodnoty nastavení
*/
gui.setSettingsValue = function(name, value) {
  api.setSettingsValue(name, value, (result, resultCode, errorMessage) => {
    gui.handleError(resultCode, errorMessage, true);
    console.log(`set ${name} ${value}`);  
  });
}

/*
  Nastavení hodnoty oprávnění
*/
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
      
      gui.validate(resultCode == "OK", null, "Objednáno", 5000);
    }
  });
}

/*
  Provede platbu
*/
gui.pay = function(username, count, callback) {
  notify.message();
  gui.setInProgress(true);
  let afterPay = (result, resultCode, errorMessage) => {
    let isOK = gui.handleError(resultCode, errorMessage);    
    if (callback) {
      callback(result, !isOK);
    }
    
    gui.setInProgress(false);
  };
  
  if (username) {
    api.payForUser(username, count, afterPay);
  } else {
    api.pay(count, afterPay);
  }
}

/*
  Načtení uživatelů
*/
gui.getUsers = function(callback) {
  api.getUsers((result, resultCode, errorMessage) => {
    if (gui.handleError(resultCode, errorMessage, true)) {
      if (callback) {
        callback(result);
      }
    }
  });
}

/*
  Smazání uživatele
*/
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

/*
  Aktivace nového uživatele
*/
gui.activateUser = function(username, callback) {
  api.activateUser(username, (result, resultCode, errorMessage) => {
    let isError = true;
    if (gui.handleError(resultCode, errorMessage, true)) {
      isError = false;
    }
    
    if (callback) {
      callback(result, isError);
    }
  });
}

/*
  Reset hesla
*/
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

/*
  Obsluha změny inputu
*/
gui.onInputChange = function(targetSelector, callback) {
  $(targetSelector).change(function() {
    let val = null;
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

/*
  Spuštění kalibrace
*/
gui.startCalibration = function(callback) {
  gui.setInProgress(true);
  api.startCalibration((result, resultCode, errorMessage) => {
    if (gui.handleError(resultCode, errorMessage, true)) {
      if (callback) {
        callback(result);
      }
    }
    
    gui.setInProgress(false);
  });
}

/*
  Ukončení kalibrace
*/
gui.stopCalibration = function(callback) {
  api.stopCalibration((result, resultCode, errorMessage) => {
    if (gui.handleError(resultCode, errorMessage, true)) {
      if (callback) {
        callback(result);
      }
    }
  });
}

/*
  Metoda pro volání api metod
*/
gui.callApiAction = function(apiMethod, callback) {
  gui.setInProgress(true);
  apiMethod((result, resultCode, errorMessage) => {
    if (gui.handleError(resultCode, errorMessage, true)) {
      if (callback) {
        callback(result);
      }
    }
    
    gui.setInProgress(false);
  });
}

/*
  Připojení k AP
*/
gui.setWifiConnection = function(ssid, securityKey, callback) {
  gui.setInProgress(true);
  api.setWifiConnection(ssid, securityKey, (result, resultCode, errorMessage) => {
    if (gui.handleError(resultCode, errorMessage, true)) {
      if (callback) {
        callback(result);
      }
    }
    
    gui.setInProgress(false);
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

/*
  Odstraní dočasně nastavené třídy
*/
gui.removeTemporaryClasses = function() {
  $(".not-displayed-temporary").removeClass("not-displayed-temporary");
  $(".displayed-temporary").removeClass("displayed-temporary");  
}

/*
  Nastavý výchozí hodnoty inputů
*/
gui.resetDefaultValues = function() {
  $("[data-defval]").each(function() {
    $(this).val($(this).data('defval'));
  });
}

///////////Metody jednotlivých stránek. Získají vstupní hodnoty a volají metody gui//////////////////////////
let firstRegistration = {};
firstRegistration.createFirstAdmin = function() {
  gui.createFirstAdmin($('#name').val(), $('#password').val(), $('#password-verification').val(), 'settings.html');
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
  let username = payment.getUsername();
  let apiMethod = username ? (callback) => {api.getUserBillCount(username, callback)} : (callback) => {api.getCurrentUserBillCount(callback)};
  gui.loadValue(apiMethod, "#count", (count, isError) => {
    if (!isError) {
      gui.setInProgress(false);
    }
  });
}

payment.getUsername = function() {
  return $(location).attr('hash').replace("#", '') || '';
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
    if (item.active) {
      row += users.generateButtonCell(item, 'Reset hesla', 'passwordReset');
    } else {
      row += users.generateButtonCell(item, 'Aktivovat', 'activate');
    }
    
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
    break
    case 'activate':
      users.activateUser(username, (result, isError) => { 
        if (isError) {
          enableButton()
        }
      });
    break;
  }
}

users.pay = function(username) {
  gui.navigate('payment.html#'+ username)
}

users.getUsers = function() {
  gui.getUsers((tableRowsData) => {
   $('#users-table-holder').html(users.createTable(tableRowsData));
  });  
}

users.loadAllowPayment = function() {
  gui.loadSettingsValue('nuserpaymnt', '#allow-payment');
}

users.onAllowPaymentCheckboxChange = function(event) {
  let checkbox = event.target;
  let status = checkbox.checked;
  gui.setSettingsValue('nuserpaymnt', status); 
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

users.activateUser = function(username, callback) {
  gui.activateUser(username, (result, isError) => {
    if(!isError) {
      users.getUsers();
    }
    
    if (callback) {
      callback(result, isError);
    }   
  });
}

users.onPageShow = function() {
  users.getUsers();
  users.loadAllowPayment();
}

let settings = {};

settings.loadNetworkInfo = function() {
  gui.loadValue(api.getGatewayIP, '#gateway');
  gui.loadValue(api.getIP, '#ip');
  gui.loadValue(api.getMAC, '#mac');
}

settings.registerChange = function() {
  gui.onInputChange('#pcount', function(element, val) {gui.setSettingsValue('pulseplitter',val);});
  gui.onInputChange('[name="mode"]', function(element, val) {gui.setSettingsValue('mode',val);});
  gui.onInputChange('#inactivity-timeout', function(element, val) {gui.setSettingsValue('mastertimeout',val);});
  gui.onInputChange('#under-limit-timeout', function(element, val) {gui.setSettingsValue('ulimtimeout',val);});
}

settings.loadValues = function() {
  gui.loadSettingsValue('ssid', '#ssid');
  gui.loadSettingsValue('skey', '#skey');
  gui.loadSettingsValue('pulseplitter', '#pcount');
  gui.loadSettingsValue('mode', '[name="mode"]');
  gui.loadSettingsValue('mastertimeout', '#inactivity-timeout');
  gui.loadSettingsValue('ulimtimeout', '#under-limit-timeout');
  settings.loadNetworkInfo();
}

settings.onPageShow = function() {
  gui.stopCalibration();
  settings.registerChange();
  settings.loadValues();
}

settings.startCalibration = function() {
  gui.startCalibration(() => {
    $("#pcount").prop('disabled', true);
    $("#pcount").val("");
    settings.toggleCalibration = settings.stopCalibration;
    $("#button-calibrate").val("Hotovo");  
  });
}

settings.stopCalibration = function() {
  gui.setInProgress(true);
  gui.stopCalibration(() => {   
    gui.loadSettingsValue('pcount', '#pcount', () => {
      settings.toggleCalibration = settings.startCalibration;
      $("#button-calibrate").val($("#button-calibrate").data('defval'));
      gui.setInProgress(false); 
    });
  }); 
}

settings.toggleCalibration = settings.startCalibration;

settings.runTest = function() {
  gui.callApiAction(api.runTest);
}

settings.restart = function() {
  gui.callApiAction(api.restart, () => {
    setTimeout(() => {
      gui.setInProgress(true)}
    );
    
    setTimeout(() => {
      gui.setInProgress(false);
      location.reload();
    }, 4000);
  });
}

settings.setWifiConnection = function() {
  gui.setWifiConnection($("#ssid").val(), $("#skey").val(), (result) => {
    gui.setInProgress(true);
    notify.alert('Zařízení bude restartováno.', () => {
      setTimeout(() => {
        gui.setInProgress(false);
        //Tady navigace na doménu. Pokud je nastavena!!!
        //let target = `http://${result}`;
        //gui.navigate(target);
      }, 4000);
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
//Přihlášeného uživatele vždy místo login page směrovat na orders - vyřešit asi rovnou na serveru?
//favicon
//Neaktivním uživatelům nedovolit žádnou akci - ošetřit na serveru