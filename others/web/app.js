// disable long press to right click
window.addEventListener("contextmenu", function(e) { e.preventDefault(); })
/*

Mobile browsers handle touch events differently than desktop browsers. While mousedown and mouseup work on desktops, they don't always translate to mobile. Instead, you can use touchstart and touchend events for mobile compatibility. Here’s how to adapt the code:
*/

// I also Had to remove the text inside the buttons, because it continue auto-pressing when the text are highlighted, using user-select: none; in css can prevent that
const 
connectBtn = document.getElementById('connectBtn'),
  connectionStatus = document.getElementById('status'),
  userFeedback = document.querySelector("#user-feedback"),
  rightBtn = document.querySelector("#btnRight"),
  leftBtn = document.querySelector("#btnLeft"),
  forwardBtn = document.querySelector("#btnForward"),
  reverseBtn = document.querySelector("#btnReverse"),
  sliderValue = document.getElementById('sliderValue'),
  controlFeedback = document.querySelector("#controlFeedback")
  ;
  
  let directionBtnPressed = "x", feedbackIndex = 0, receivedFeedback = "", controlMsg = "";
  
  let ws;


const clearControlFeedback = () => {
  controlFeedback.innerHTML = `<i></i>`;
}


function isWsConnected () {
  return ws && ws.readyState === WebSocket.OPEN;
}


let intervalId;
const handleControl = (feedback) => {
  clearInterval(intervalId);
  clearControlFeedback();
  intervalId = setInterval(() => {
    if(isWsConnected()) {
      logg(1);
      sendControlMsg();
      logg(feedback);
      controlFeedback.innerHTML += `<div class="scroll-text">${receivedFeedback}</div>`;
    } else {
      logg("not connected");
      controlFeedback.innerHTML += `<div class="scroll-text" style="color:red;">Not connected</div>`;
    }
  }, 100);// send every 100 milliseconds
}

const stopControlMsg = () => {
  clearInterval(intervalId);
  clearControlFeedback();
  directionBtnPressed = "x";
  isWsConnected() && sendControlMsg();
}

const logg = (msg) => {
  // console.log(msg);
}

const getBtnString =  () => {
  return "directionBtnPressed = " + directionBtnPressed + "\nfull control signal is " +  directionBtnPressed;
} 


let temperature, heartRate;
const add = true, remove = false;

function alertUserLoading(addOrRemove) {
  userFeedback.innerHTML = (addOrRemove) ?`Connecting......`: ``;
}


const sendControlMsg = () => {
  if (isWsConnected) {
    controlMsg = directionBtnPressed;
    ws.send(controlMsg);
    logg("control msg sent: " + "\"" + controlMsg + "\"");
  } else {
    logg("Unable to send message: WebSocket not connected.");
    handleDisconnection();
  }
}

const rightBtnHandler = () => {
  directionBtnPressed = "r";
  handleControl(getBtnString());
};

const leftBtnHandler = () => {
  directionBtnPressed = "l";
  handleControl(getBtnString());
};

const forwardBtnHandler = () => {
  directionBtnPressed = "f";
  handleControl(getBtnString());
};

const reverseBtnHandler = () => {
  directionBtnPressed = "b";
  handleControl(getBtnString());
};

const handleDisconnection = () => {
  connectionStatus.textContent = 'Status: Not connected';
  connectionStatus.className = "not-connected";
  // remove the "connecting.....'
  alertUserLoading(remove);
  logg('Disconnected from WebSocket server');
}

// Event listeners for desktop and mobile
// ================== MOUSE DOWN (WHEN CLICKED ON)
connectBtn.addEventListener("click", connectToSocket);
rightBtn.addEventListener("mousedown", rightBtnHandler);
leftBtn.addEventListener("mousedown", leftBtnHandler);
forwardBtn.addEventListener("mousedown", forwardBtnHandler);
reverseBtn.addEventListener("mousedown", reverseBtnHandler);

// ===================== MOUSE UP (WHEN DROPPED AFTER BEING CLICKED ON)
rightBtn.addEventListener("mouseup", stopControlMsg);
leftBtn.addEventListener("mouseup", stopControlMsg);
forwardBtn.addEventListener("mouseup", stopControlMsg);
reverseBtn.addEventListener("mouseup", stopControlMsg);

// ===================== MOUSE LEAVE (WHEN MOUSE ISN'T HOVERING)
rightBtn.addEventListener("mouseleave", stopControlMsg);
leftBtn.addEventListener("mouseleave", stopControlMsg);
forwardBtn.addEventListener("mouseleave", stopControlMsg);
reverseBtn.addEventListener("mouseleave", stopControlMsg);

// FOR COMPATIBILITY WITH MOBILES:
//====================== TOUCH START AND TOUCH END, INCASE MOUSEUP AND MOUSEDOWN DOESN'T WORK ON SOME MOBILE
rightBtn.addEventListener("touchstart", rightBtnHandler);
leftBtn.addEventListener("touchstart", leftBtnHandler);
forwardBtn.addEventListener("touchstart", forwardBtnHandler);
reverseBtn.addEventListener("touchstart", reverseBtnHandler);
// ===================
rightBtn.addEventListener("touchend", stopControlMsg);
leftBtn.addEventListener("touchend", stopControlMsg);
forwardBtn.addEventListener("touchend", stopControlMsg);
reverseBtn.addEventListener("touchend", stopControlMsg);

function connectToSocket() {
  logg("connecting");
  
  const ipAddress = document.getElementById('ipAddress').value;
  const wsUrl = `ws://${ipAddress}:81/`; // Connect to WebSocket server at ESP32 IP and port 81
  
  // check if ipAddress is empty
  if(!ipAddress) {
    userFeedback.innerHTML = `<span style="color:red;">ESP32 IP Address must be entered to establish connection</span>`
    return;
  }

  alertUserLoading(add);

  ws = new WebSocket(wsUrl); //

  ws.onopen = function() {//once connection is established
    connectionStatus.textContent = 'Status: Connected';
    connectionStatus.className = "connected";
    alertUserLoading(remove);
    logg('Connected to WebSocket server');
  };

  ws.onmessage = function(event) { //once a new message is received
    receivedFeedback = event.data;
  };

  ws.onclose = function() { //once the connection is closed
    handleDisconnection();
  };

  ws.onerror = function() { //once error is encountered in connection
    connectionStatus.textContent = 'Status: Connection Error, try again';
    logg('Error connecting to WebSocket server');
    userFeedback.innerHTML = `<div style="color:red;">Error: Unable to connect</div>`;
  };
}

