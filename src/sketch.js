const char SKETCH_JS[] PROGMEM = R"=====(


let angle="";
let distance="";
let data="";
let noObject;
let pixsDistance;
let iAngle, iDistance;
let index1=0;
let index2=0;
let orcFont;

var thickness
var thickness2



//---------websocket stuff-------//

var websocket

var value

//console.log('BRUUUUUH')

// const websocket2= new WebSocket('ws://' + window.location.hostname + ':82/')
// //console.log("ws://" + window.location.hostname + ":82/");
// console.log(websocket2)

// websocket2.addEventListener("open", (event)=>{
//   websocket2.send("TEST2")
// })

// websocket2.addEventListener("message", (event) => {
//   console.log("Message from server ", event.data);
// });



function wsConnect() {

  
  // start a WebSocket
  websocket = new WebSocket('ws://' + window.location.hostname + ':81/'); //note this is just appearing yellow bc of my weird comment notation thing
  console.log("ws://" + window.location.hostname + ":81/");
    
  websocket.onopen = function(event) { onOpen(event) }
  websocket.onclose = function(event) { onClose(event) }
  websocket.onmessage = function(event) { onMessage(event) } //this is the important one, this is the one handling the data
  websocket.onerror = function(event) { console.log(event.data); }
}


function onOpen(event){
  console.log('websocket opened ' + event.data);
}

function onClose(event){
  console.log('websocket closed ' + event.data);
  wsConnect();
}

function onMessage(event){
  console.log('websocket data: ' + event.data);
  value = String(event.data);

  //two numbers code

  var temp = value.split(',');

  iAngle = parseInt(temp[0])
  iDistance = parseInt(temp[1])

  //value3 = parseInt(value1);
  //value4 = parseInt(value2);



  //console.log('value3 ' + value3 + ' value4 ' + value4);

  //thickness = value3
  //thickness2 = value4

  //iAngle = value3
  //iDistance = value4

  //console.log(iAngle);
  //console.log(iDistance);
  //console.log("WEEE")


  //
  //value = parseInt(value); //use parseInt to convert the string into an integer
  //receivedData(value);
}


wsConnect()//CALLED wsConnect to get all the websocket stuff started

console.log(websocket)



//----------------p5.js stuff----------------//

//this goes with the websocket stuff really, not sure if it needs to be down here for some reason
// function receivedData(data) {
//   //var value = map(data, 2200, 2800, 0, 30);

//   // var value = data //i think getting two values so value might need to be an array
//   // thickness = value;
//   // console.log(thickness)


// }





function setup() {
  //createCanvas(400, 400);
  //createCanvas(windowWidth, windowHeight);
  createCanvas(1200,700);// this sets width=1200, height=700
  smooth()



}





function draw() {
  background(0);//background is a bit misleading name, it really just means cover the canvas in this color
  //fill(98,245,31) //bright green

  //fill(0,4)
  //rect(0, 0, width, height-height*0.065);
  //stroke(200,0,0)
  //stroke(98,245,31)
  //line(0,0,1200,700)

  //translate(600,675) //if i want to manually position,this is probly the good one

  stroke(98,245,31)
  //line(thickness,0,200,200)
  //text(value,20,20)

  drawRadar()
  drawLine()
  drawObject()
  drawText()

  //test1()
  //line(0,0,400,400)
  //text(thickness,20,20)


  //stroke(98,245,31)
  //fill(98,245,31)
  //text(thickness,400,400)

  
  //console.log(thickness+100)

  // line(thickness+300,500,400,400)
  // line(thickness2+300,500,400,400)
  //console.log(iAngle)
  //console.log(iDistance)


}


// //this goes with the websocket stuff really, not sure if it needs to be down here for some reason
// function receivedData(data) {
//   //var value = map(data, 2200, 2800, 0, 30);
//   var value = data //i think getting two values so value might need to be an array
//   var thickness = value;
//   console.log(thickness)
// }



// function test1() {
//   //stroke(98,100,31);
//   //line(0,30,400,400)
//   background(0)
// }

//draw arc and angle lines
function drawRadar() {
  push();
  translate(width/2,height-height*0.074); // moves the starting coordinates to new location
  //translate(600,675)
  noFill();
  strokeWeight(2);//thicken lines
  stroke(98,245,31);//set color for lines to green
  
  // draws the arc lines
  arc(0,0,(width-width*0.0625),(width-width*0.0625),PI,TWO_PI);//want perfect circles so set arc ellipse width and height to same value (write both as fuctions of window width, 0.0625 is just an arbitrary scaling factor)
  arc(0,0,(width-width*0.27),(width-width*0.27),PI,TWO_PI);//PI and TWO_PI are angles to start and stop the arc, not sure why can't use 0
  arc(0,0,(width-width*0.479),(width-width*0.479),PI,TWO_PI);
  arc(0,0,(width-width*0.687),(width-width*0.687),PI,TWO_PI);//draws the innermost arc
  
  // draws the angle lines
  line(-width/2,0,width/2,0);//draw horizontal line at base of radar
  line(0,0,(-width/2)*cos(radians(30)),(-width/2)*sin(radians(30)));//use trig to get coordinate for 2nd point on line
  line(0,0,(-width/2)*cos(radians(60)),(-width/2)*sin(radians(60)));
  line(0,0,(-width/2)*cos(radians(90)),(-width/2)*sin(radians(90)));
  line(0,0,(-width/2)*cos(radians(120)),(-width/2)*sin(radians(120)));
  line(0,0,(-width/2)*cos(radians(150)),(-width/2)*sin(radians(150)));
  line((-width/2)*cos(radians(30)),0,width/2,0);
  pop();
}

//draws the red blocks that represent the object seen by the ultrasonic sensor
function drawObject() {
  push();
  translate(width/2,height-height*0.074); // moves the starting coordinates to new location
  strokeWeight(9);
  stroke(255,10,10); // red color
  pixsDistance = iDistance*((height-height*0.1666)*0.025); // converts the distance from the sensor from cm to pixels
  // limiting the range to 40 cms
  if(iDistance<40){
    // draws the object according to the angle and the distance
    //draws the red line from start of the object to end
  line(pixsDistance*cos(radians(iAngle)),-pixsDistance*sin(radians(iAngle)),(width-width*0.505)*cos(radians(iAngle)),-(width-width*0.505)*sin(radians(iAngle)));
  }
  pop();
}


function drawLine() {
  push();
  strokeWeight(9);
  stroke(30,250,60);//sets the color
  translate(width/2,height-height*0.074); // moves the starting coordinats to new location
  line(0,0,(height-height*0.12)*cos(radians(iAngle)),-(height-height*0.12)*sin(radians(iAngle))); // draws the line according to the angle
  pop();
}



function drawText() { 
  
  push();
  // if(iDistance>40) {
  // noObject = "Out of Range";
  // }
  // else {
  // noObject = "In Range";
  // }
  fill(0,0,0);
  noStroke();
  rect(0, height-height*0.0648, width, height);
  fill(98,245,31);
  textSize(25);
  
  text("10cm",width-width*0.3854,height-height*0.0833);
  text("20cm",width-width*0.281,height-height*0.0833);
  text("30cm",width-width*0.177,height-height*0.0833);
  text("40cm",width-width*0.0729,height-height*0.0833);
  textSize(30);
  text("RADAR ", width-width*0.875, height-height*0.0277);
  text("Angle: " + iAngle +" °", width-width*0.48, height-height*0.0277);
  text("Distance: ", width-width*0.26, height-height*0.0277);
  if(iDistance<40) {
  text("        " + iDistance +" cm", width-width*0.225, height-height*0.0277);
  }
  textSize(25);
  fill(98,245,60);
  translate((width-width*0.4994)+width/2*cos(radians(30)),(height-height*0.0907)-width/2*sin(radians(30)));
  rotate(-radians(-60));
  text("30°",0,0);
  resetMatrix();
  translate((width-width*0.503)+width/2*cos(radians(60)),(height-height*0.0888)-width/2*sin(radians(60)));
  rotate(-radians(-30));
  text("60°",0,0);
  resetMatrix();
  translate((width-width*0.507)+width/2*cos(radians(90)),(height-height*0.0833)-width/2*sin(radians(90)));
  rotate(radians(0));
  text("90°",0,0);
  resetMatrix();
  translate(width-width*0.513+width/2*cos(radians(120)),(height-height*0.07129)-width/2*sin(radians(120)));
  rotate(radians(-30));
  text("120°",0,0);
  resetMatrix();
  translate((width-width*0.5104)+width/2*cos(radians(150)),(height-height*0.0574)-width/2*sin(radians(150)));
  rotate(radians(-60));
  text("150°",0,0);
  pop(); 
}


)=====";