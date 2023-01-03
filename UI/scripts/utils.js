var notify = {};
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

var api = {};

api.createAdmin = function(username, password, callback) {
  //DEBUG Odladit speciállní znaky v hesle
}

api.createUser = function(username, password, callback) {

}

api.resetPassword = function(username, callback) {

} 

api.deleteUser = function(username, callback) {

}

api.login = function(username, password, callback) {

}

api.logout = function(callback) {

}

api.setValue = function(name, value, callback) {

}

api.getValue = function(name, value, callback) {

}

var gui = {};
gui.verify = function(isOK, errorMessage, okMessage = "", timeout = 0){
  if (isOK && okMessage) {
    notify.message(okMessage, false, timeout);
  } else if(!isOK && errorMessage) {
    notify.message(errorMessage, true, timeout);
  }
  
  return isOK;
}

gui.validateUsername = function(username) {
  var regEx = /^[0-9a-zA-Z]+$/;
  var minLen = 6;
  var maxLen = 15;
  return gui.verify(username, "Není vyplněno jméno.") && 
  gui.verify(username.length >= minLen, `Minimální délka jména je ${minLen} znaků.`) &&
  gui.verify(username.length <= maxLen, `Maximální délka jména je ${maxLen} znaků.`) && 
  gui.verify(username.match(regEx), "Jméno smí obsahovat pouze písmena a číslice.");
}

gui.validatePassword = function(password) {
  var minLen = 6;
  var maxLen = 30;
  return gui.verify(password, "Není vyplněno heslo.") && 
  gui.verify(password.length >= minLen, `Minimální délka hsela je ${minLen} znaků.`) &&
  gui.verify(password.length <= maxLen, `Maximální délka hesla je ${maxLen} znaků.`);
}

gui.verifyPassword = function(password, passwordVerification) {
  return gui.verify(password == passwordVerification, "Neshoduje se ověření hesla.");
}

gui.createAdmin = function(username, password, passwordVerification) {
  gui.validateUsername(username) &&
  gui.validatePassword(password) &&
  gui.verifyPassword(password, passwordVerification) &&
  api.createAdmin((result, errorMessage) => {
    if (errorMessage) {
      notify.error(errorMessage);
      return;
    }
    
    if (result == 'user_exists') {
      notify.message('Uživatel s tímto jménem už existuje.', true);
      return;
    }
    
    if (result == 'ok') {
      //ok přesměrování do nastavení
      return;
    }
    
    notify.error(result);
  });
}

//DEBUG
//Test nevyplněné jméno, krátké jméno, dlouhé heslo, neplatné znaky
//Test nevyplněné heslo, krátké heslo, dlouhé heslo
//Test verifikace hesla
//Test existujícího jméne
//Test chyby HTTP
//Test neplatné návratové hodnty
//Test vše ok
