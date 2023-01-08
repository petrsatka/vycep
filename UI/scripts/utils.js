/*
  Sdružuje výpisy notifikací a chyb.
*/
var notify = {};
/*
  Vypíše chybu do message boxu.
*/
notify.error = function(message) {
  $.alert({
    title: 'Chyba!',
    content: message,
    container:'.page-wrapper',
    boxWidth: '90%',
    useBootstrap: false,
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
api.getQueueCount = function(callback) {
  setTimeout(() => callback(api.qCountDEBUG,"ok", null),500);
  //setTimeout(() => callback(-1,"unable_to_get_qcount", null), 500);
  //setTimeout(() => callback(null, null, "500 - bad request"), 500); 
}

api.getUserBillCount = function(callback) {
  setTimeout(() => callback(api.billCountDEBUG,"ok", null),500);
  //setTimeout(() => callback(-1,"unable_to_get_bcount", null),500);
  //setTimeout(() => callback(null, null, "500 - bad request"),500);
}

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
      message = 'Neplatné heslo'
      break
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

gui.loadValue = function(apiMethod, targetElementSelector, callback) {
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
  });
}

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
  Maže citlivé položky
*/
gui.clearSensitiveInputs = function() {
  $('input#old-password').val("");
  $('input#name').val("");
  $('input#password').val("");
  $('input#password-verification').val("");
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

var payment = {};
payment.pay = function() {
}

payment.goBack = function() {
  passwordChange.clearValues();
  gui.navigateToReferrer();
}

payment.loadBCount = function() {
  gui.loadValue(api.getUserBillCount, "#count", (val, isError) => {
    $("#count").prop('max', val || 0);
  });
}

payment.loadValues = function() {
  $("#username").text(cookies.getUsername());
  payment.loadBCount();
}

////Global Events////
$(window).on("pageshow",function(){
  gui.clearSensitiveInputs(); 
})

//DEBUG
//Přihlášeného uživatel vždy místo login page směrovat na orders - vyřešit asi rovnou na serveru?
//favicon
//RETEST všech operací stránek s ohledem na zamykání při delším trvání odpovědi
