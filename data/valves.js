window.onload = getStationStatus();

document.getElementsByTagName('button')[0].addEventListener('click', clickStart);
$('simul').addEventListener('click', simulChecked)
simulChecked();

function $(el) {
  var x = document.getElementById(el)
  if (x){
    window.el = x
    return x
  }
}

function isEmpty(obj) {
    for(var key in obj) {
        if(obj.hasOwnProperty(key))
            return false;
    }
    return true;
}

function simulChecked() {
  var checks = document.querySelectorAll("input[type=checkbox]")
  if ($('simul').checked){
    for (i=0, j=checks.length-1; i<j; i++) {
      checks[i].removeEventListener('click', onlyOne);
      checks[i].checked = false;
    }
  }
  else {
    for (i=0, j=checks.length-1; i<j; i++) {
      checks[i].addEventListener('click', onlyOne);
      checks[i].checked = false;
    }
  }
}

function onlyOne(event){
  var checks = document.querySelectorAll("input[type=checkbox]");
  station = event.target.getAttribute('data-station') - 1;
  
  for (i=0, j=checks.length-1; i<j; i++) {
    if (i==station) {
      continue;
    }
    else {
      checks[i].checked = false;
    }
  } 
}


function clickStart(){
  var checks = document.querySelectorAll("input[type=checkbox]");
  var program ={timers:[]};
  for (i=0, j=checks.length-1; i<j; i++) {
    if (checks[i].checked){
      var number = i+1;
      var station = "s" + number;
      var timer = $(station + "minutes").value * 60 * 1000 + $(station + "seconds").value * 1000;
      program.timers[i] = timer;
    }  
    
    if (!checks[i].checked){
      var number = i+1;
      var timer = 0;
      program.timers[i] = timer;
    }  
  }
  
  var hr = new XMLHttpRequest();	
  var url = "manual_control";
  var vars = 'json=' + JSON.stringify(program);
  hr.open("POST", url, true);
  hr.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
  hr.send(vars);
  //console.log (hr);
  hr.onreadystatechange = function() {
    if(hr.readyState == 4 && hr.status == 200) {
      var return_data = hr.responseText;
      //console.log (return_data);	
      getStationStatus();
      if (return_data !== '') {
                                      
      }
                                            
    }
  }
}

function getStationStatus(){
  var hr = new XMLHttpRequest();	
  var url = "station_status";
  hr.open("POST", url, true);
  hr.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
  hr.send();
  //console.log (hr);
  hr.onreadystatechange = function() {
    if(hr.readyState == 4 && hr.status == 200) {
      stationStatus = JSON.parse(hr.responseText);
      console.log (status);	
      for(i=0; i<8; i++){
        var station = i+1;
        station = "s" + station;
        if (stationStatus.stationOn[i] === 1) {
          $(station + "Status").innerText = 'On';
        } else {
          $(station + "Status").innerText = 'Off';
        }
        
        if (i<7 && stationStatus.offTimer[i]>0){
          displayTime(station + "timer", Math.ceil(stationStatus.offTimer[i]/1000)); 
        }
      }
      counter = setInterval(updateTimer, 1000);
    }
  }
}

function updateTimer(){
  var isRunning = false;
  for(i=0; i<7; i++){
   var station = i+1;
   station = "s" + station;
   if (stationStatus.offTimer[i]>0){
          stationStatus.offTimer[i] -= 1000;
          displayTime(station + "timer", Math.ceil(stationStatus.offTimer[i]/1000)); 
          isRunning = true;
    } 
    
    if (!isRunning){
      clearInterval(counter);
      $(station + "Status").innerText = 'Off';
      $(station + "timer").innerText = '';
      $('s8Status').innerText = 'Off';
    }
  }
}

function displayTime(el, elTime) {
        var sec = elTime	
        var hours = Math.floor(sec / 3600);
        sec -= hours*3600;
        var mins = Math.floor(sec / 60);
        sec -= mins*60;
        
        if (hours < 1) {
            $(el).innerHTML = mins + ':' + LeadingZero(sec);
        } else {
            $(el).innerHTML = hours + ':' + LeadingZero(mins) + ':' + LeadingZero(sec);
          }	
}

function LeadingZero(Time) {
	return (Time < 10) ? '0' + Time : +Time;
}

