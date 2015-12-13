/* (c) LGPL 2015 - Steve Schnepp <steve.schnepp@pwkf.org> */

// Create the canvas
var canvas = document.getElementById("myCanvas");
var ctx = canvas.getContext("2d");
canvas.width = 640;
canvas.height = 480;

// Space is black
ctx.fillStyle = "black";
ctx.fill();

// Game objects
var ORBITAL_RATE = 100;

var player = {
	mass: 26, // mass
	next_orbital_object: ORBITAL_RATE,
	orbital_speed : 300,
};

var orbitals = [];
var looses = [];

// Handle keyboard controls
var keysDown = {};

addEventListener("keydown", function (e) {
	keysDown[e.keyCode] = true;
}, false);

addEventListener("keyup", function (e) {
	delete keysDown[e.keyCode];
}, false);

function getOrbitalX(orbital) {
	return Math.cos(orbital.theta) * orbital.distance;
}
function getOrbitalY(orbital) {
	return Math.sin(orbital.theta) * orbital.distance;
}

// Update game object
function update(modifier) {
	if (38 in keysDown) {
		// KEY_UP : accelerate orbitals by moving them closer to planet
		var idx;
		for (idx = 0; idx < orbitals.length; idx++) {
			var orbital = orbitals[idx];
			orbital.distance *= 0.95;
		}
	}

	if (40 in keysDown) {
		// KEY_DWN : release all orbitals
		var idx;
		for (idx = 0; idx < orbitals.length; idx++) {
			var orbital = orbitals[idx];
			orbital.x = getOrbitalX(orbital);
			orbital.y = getOrbitalY(orbital);
			orbital.speed = Math.PI * orbital.distance / 100;
			orbital.theta += Math.PI / 2;

			looses.push(orbital);
		}
		// We just emptied orbitals
		orbitals = [];
	}

	// Add a new orbital every orbital_rate;
	if (! (player.next_orbital_object --)) {
		player.next_orbital_object = ORBITAL_RATE;

		var orbital = {
			attached: true,
			mass: 10,
			distance: 100,

			theta: Math.random() * Math.PI,
		};

		orbitals.push(orbital);
	}


	// Update all the game objects
	var idx;
	for (idx = 0; idx < orbitals.length; idx++) {
		var orbital = orbitals[idx];
		orbital.theta += Math.PI / orbital.distance;
	}

	var idx;
	for (idx = 0; idx < looses.length; idx++) {
		var loose = looses[idx];
		loose.x += loose.speed * Math.cos(loose.theta);
		loose.y += loose.speed * Math.sin(loose.theta);
	}

	// Adding asteroids on each side
	var asteroid = { };
	asteroid.x = 1000;
	asteroid.y = 1000;
	asteroid.theta = Math.random() * Math.PI;
	asteroid.speed = Math.random() * 10;

	// Update collisions
	// TODO
};

function drawCircle(x, y, radius) {
	ctx.beginPath();
	ctx.arc(canvas.width/2 + x, canvas.height/2 + y, radius/10, 0, 2 * Math.PI);
	ctx.fill();
}

function render() {

	ctx.fillStyle = "black";
	ctx.fillRect(0, 0, canvas.width, canvas.height);

        var idx;
        for (idx = 0; idx < orbitals.length; idx++) {
                var orbital = orbitals[idx];
		var x = getOrbitalX(orbital);
		var y = getOrbitalY(orbital);

		ctx.fillStyle = "yellow";
		drawCircle(x, y, orbital.mass);
        }

        var idx;
        for (idx = 0; idx < looses.length; idx++) {
                var loose = looses[idx];

		ctx.fillStyle = "white";
		drawCircle(loose.x, loose.y, loose.mass);
        }

	// Player
	ctx.fillStyle = "red";
	drawCircle(0, 0, player.mass);
}

// The main game loop
var main = function () {
	var now = Date.now();
	var delta = now - then;

	update(delta / 1000);
	render();

	then = now;

	// Request to do this again ASAP
	requestAnimationFrame(main);
};

// Cross-browser support for requestAnimationFrame
var w = window;
requestAnimationFrame = w.requestAnimationFrame || w.webkitRequestAnimationFrame || w.msRequestAnimationFrame || w.mozRequestAnimationFrame;

// Let's play this game!
var then = Date.now();
main();
