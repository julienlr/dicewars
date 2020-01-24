"use strict";

const WebSocketUrl = window.location.host + "/ws";
const ProtocolVersion = 1;

var ws;
function webSocketStart() {
	try {
		ws = new WebSocket("ws://" + WebSocketUrl);
	} catch (exception) {
		console.error(exception);
	}
	if (!ws) return;

	ws.onclose = function (evt) {
		console.log("close:\n" + evt.data);
	};
	ws.onerror = function (evt) {
		console.log("error:\n" + evt.data);
	};
	ws.onmessage = function (evt) {
		console.log("message:\n");
		try {
			var jsonObj = JSON.parse(evt.data);
			parseJsonMsg(jsonObj);
		}
		catch (e) {
			console.error("Parsing error:", e);
		}
	};
	ws.onopen = function (evt) {
		console.log("open:\n" + evt.data);
		if (ws) ws.send(genMessage("Init"));
	};
}

function parseJsonMsg(data) {
	if (!data.name || data.name != "DiceWars"
		|| !data.version || data.version != ProtocolVersion
		|| !data.data)
		return;

	parseData(data.data);
}

var currentMap = null;

function parseData(data) {
	if (!Array.isArray(data)) data = [data];

	for (var msg in data) {
		for (var d in data[msg]) {
			switch (d) {
				case "map":
					currentMap = data[msg][d];
					initMap();
					break;

				case "players":
					if (Array.isArray(data[msg][d])) {
						for (var i in data[msg][d]) {
							if (data[msg][d][i].player)
								initPlayer(data[msg][d][i].player);
						}
					}
					break;

				case "player":
					initPlayer(data[msg][d]);
					break;

				case "state":
					setState(data[msg][d]);
					break;
			}
		}
	}
}

function genMessage(val) {
	return (JSON.stringify({ "name": "DiceWars", "version": ProtocolVersion, "request": val }));
}

function webSocketClose() {
	if (ws) {
		ws.close();
		ws = null;
	}
}

var isLoaded = false;
var toLoad = [];
function waitLoad(func, _this, args) {
	if (isLoaded) return (false);
	toLoad.push({ "func": func, "this": _this, "args": args });
	return (true);
}

function endLoad(evt) {
	isLoaded = true;
	evt.target.parentNode.style.display = "none";
	svgDice = evt.target.parentNode.appendChild(evt.target.contentDocument.querySelector("svg").cloneNode(true));
	evt.target.parentNode.removeChild(evt.target);
	while (toLoad.length) {
		var f = toLoad.shift();
		f.func.apply(f.this, f.args);
	}
}

const strokePoints = [[3, 4], [4, 5], [2, 3], [0, 5], [1, 2], [0, 1]];
var svgContent = null;
var svgDice = null;
var mapContent = null;
var playerContent = null;
var infoContent = null;
function initMap() {
	initGrid(currentMap.width, currentMap.height);

	for (var r in currentMap.regions) {
		var nb = 0;
		currentMap.regions[r].center = { x: 0, y: 0 };
		var regionObj = document.createElementNS('http://www.w3.org/2000/svg', 'g');
		regionObj.setAttribute('id', "region_" + r);
		currentMap.regions[r].obj = regionObj = mapContent.querySelector("#map").appendChild(regionObj);
		regionObj.setAttribute("stroke-opacity", "0.1");
		regionObj.setAttribute("fill", "#AAA");
		var regionAnim = document.createElementNS('http://www.w3.org/2000/svg', 'animate');
		currentMap.regions[r].anim = regionAnim;
		regionAnim.setAttribute("id", "anim");
		regionAnim.setAttribute("attributeName", "fill-opacity");
		regionAnim.setAttribute("values", "0.7 ; 0 ; 0.7 ; 0 ; 0.7");
		regionAnim.setAttribute("keyTimes", "0 ; 0.25 ; 0.5 ; 0.75 ; 1");
		regionAnim.setAttribute("dur", "1.5s");
		regionAnim.setAttribute("begin", "none");
		regionObj.appendChild(regionAnim);

		for (var c in currentMap.regions[r]) {
			var cell = currentMap.regions[r][c];
			var cellObj = document.getElementById('cell_' + cell.x + '_' + cell.y);
			if (!cellObj) continue;
			cellObj = regionObj.appendChild(cellObj.parentNode.removeChild(cellObj));
			cellObj.region = (parseInt(r) + 1);
			currentMap.regions[r].center.x += parseInt(cellObj.getAttribute('_x'));
			currentMap.regions[r].center.y += parseInt(cellObj.getAttribute('_y'));
			++nb;
		}
		if (nb != 0) {
			currentMap.regions[r].center.x /= nb;
			currentMap.regions[r].center.y /= nb;
		}
	}

	for (var r in currentMap.regions) {
		for (var c in currentMap.regions[r]) {
			var cell = currentMap.regions[r][c];
			var cellObj = document.getElementById('cell_' + cell.x + '_' + cell.y);
			if (!cellObj) continue;
			cellObj.parsed = true;

			var i = 0;
			for (var y = cell.y - 1; y < cell.y + 2; ++y) {
				for (var _x = 0; _x < 2; ++_x) {
					var x = cell.x - 1;
					if (y == cell.y) x += _x * 2;
					else x += _x + (y + 1) % 2;

					var draw = false;
					if (x < 0 || y < 0 || x >= currentMap.width || y >= currentMap.height) draw = true;
					else {
						var cellObj2 = document.getElementById('cell_' + x + '_' + y);
						if (cellObj2 && !cellObj2.parsed && cellObj2.region != cellObj.region) draw = true;
					}

					if (draw) {
						var line = document.createElementNS('http://www.w3.org/2000/svg', 'line');
						line.setAttribute('x1', cellObj.data[strokePoints[i][0]].x);
						line.setAttribute('y1', cellObj.data[strokePoints[i][0]].y);
						line.setAttribute('x2', cellObj.data[strokePoints[i][1]].x);
						line.setAttribute('y2', cellObj.data[strokePoints[i][1]].y);
						line.setAttribute("stroke", "black")
						mapContent.querySelector("#map").appendChild(line);
					}

					++i;
				}
			}
		}
	}
}

const cellSize = { w: 30, h: 20 };
const gameOffset = { x: 10, y: 30 };
var gameSize = { w: 0, h: 10 };
function initGrid(width, height) {
	var content = document.getElementById('content');
	if (!content) return (false);

	content.innerHTML = "";
	var svgDiceDiv = document.createElement("div");
	svgDiceDiv = content.appendChild(svgDiceDiv);
	svgDice = document.createElement("object");
	svgDice.onload = endLoad;
	svgDice.setAttribute("id", "svgdice");
	svgDice.setAttribute("type", "image/svg+xml");
	svgDice.setAttribute("data", "img/dice.svg");
	svgDice.setAttribute("display", "none");
	svgDice = svgDiceDiv.appendChild(svgDice);

	svgContent = document.createElementNS("http://www.w3.org/2000/svg", "svg");
	svgContent.setAttribute("width", "100%");
	svgContent.setAttribute("height", "100%");

	mapContent = document.createElementNS("http://www.w3.org/2000/svg", "g");
	mapContent.setAttribute("id", "mapcontent");
	var mapNode = document.createElementNS("http://www.w3.org/2000/svg", "g");
	mapNode.setAttribute("id", "map");
	mapNode.setAttribute("fill", "#EEE");
	mapNode.setAttribute("stroke", "#DDD");
	mapContent.appendChild(mapNode);
	var stateNode = document.createElementNS("http://www.w3.org/2000/svg", "g");
	stateNode.setAttribute("id", "state");
	mapContent.appendChild(stateNode);
	svgContent.appendChild(mapContent);

	var offsetx;
	for (var i = 0; i < width; ++i) {
		for (var j = 0; j < height; ++j) {
			if (j % 2) offsetx = gameOffset.x + cellSize.w / 2.0;
			else offsetx = gameOffset.x;
			var _x = i * cellSize.w + cellSize.w / 2.0;
			var _y = j * (cellSize.h * (1 - (1 - Math.sin(Math.PI / 6)) / 2)) + cellSize.h / 2.0;
			mapNode.appendChild(genHexagonSVG(_x + offsetx, _y + gameOffset.y, cellSize.w, cellSize.h, 'cell_' + i + '_' + j));

			_x += cellSize.w / 2.0;
			_y += cellSize.h / 2.0
			if (gameSize.w < _x) gameSize.w = _x;
			if (gameSize.h < _y) gameSize.h = _y;
		}
	}

	var infoNodeContent;
	infoContent = document.createElementNS("http://www.w3.org/2000/svg", "g");
	infoContent.setAttribute("id", "infocontent");
	infoContent.setAttribute("transform", "translate(0," + (gameOffset.y + gameSize.h + cellSize.h / 2) + ")");
	infoNodeContent = document.createElementNS("http://www.w3.org/2000/svg", "text");
	infoNodeContent.setAttribute("id", "text");
	infoNodeContent.setAttribute("text-anchor", "end");
	infoNodeContent.setAttribute("x", (gameOffset.x + gameSize.w + cellSize.w / 2.0) / 2.0 - cellSize.w * 2);
	infoNodeContent.setAttribute("y", cellSize.h * 1.8);
	infoNodeContent.setAttribute("font-size", 25);
	infoNodeContent.setAttribute("font-family", "Verdana");
	infoNodeContent.setAttribute("font-weight", "bold");
	infoContent.appendChild(infoNodeContent);
	infoNodeContent = document.createElementNS("http://www.w3.org/2000/svg", "g");
	infoNodeContent.setAttribute("id", "dices");
	infoNodeContent.setAttribute("transform", "translate(" + ((gameOffset.x + gameSize.w + cellSize.w / 2.0) / 2.0) + "," + (2 * cellSize.h / 3.0) + ")");
	infoContent.appendChild(infoNodeContent);
	svgContent.appendChild(infoContent);

	playerContent = document.createElementNS("http://www.w3.org/2000/svg", "g");
	playerContent.setAttribute("id", "playercontent");
	svgContent.appendChild(playerContent);

	content.appendChild(svgContent);
}

function genHexagonSVG(x, y, w, h, id) {
	var node = document.createElementNS('http://www.w3.org/2000/svg', 'polygon');

	var val = "";
	node.data = [];

	w /= Math.cos(Math.PI / 6);
	for (var i = 0; i < 6; ++i) {
		var angle = Math.PI / 3 * i + Math.PI / 6;
		if (i) val += " ";
		var obj = { x: Math.cos(angle) * w / 2 + x, y: Math.sin(angle) * h / 2 + y };
		val += obj.x + "," + obj.y;
		node.data.push(obj);
	}
	node.setAttribute("points", val);
	node.setAttribute('id', id);
	node.setAttribute('_x', x);
	node.setAttribute('_y', y);

	return (node);
}

function setRegionData(id, color, dices) {
	if (waitLoad(setRegionData, this, arguments)) return;

	var stateObj = document.getElementById('state');
	if (!stateObj) return;
	var region = currentMap.regions[id];
	if (!region) return;

	region.obj.setAttribute("fill", color);
	region.obj.setAttribute("fill-opacity", "0.7");

	var diceObj = stateObj.querySelector("#dice_" + id);
	if (!diceObj) {
		diceObj = document.createElementNS('http://www.w3.org/2000/svg', 'use');
		//diceObj = svgDice.contentDocument.getElementById('dicef6')
		diceObj.setAttribute("id", "dice_" + id);
		diceObj.setAttribute("x", region.center.x);
		diceObj.setAttribute("y", region.center.y);
		stateObj.appendChild(diceObj);
	}
	diceObj.setAttribute("href", "#dice" + dices);
	diceObj.setAttribute("fill", color);
}

var Players = [];
const PlayersColor = ["orange", "purple", "red", "green", "yellow", "lightgreen", "pink", "lightblue"];
function initPlayer(player) {
	if (!"id" in player) return;
	Players[player.id] = player;
	drawPlayers();
}

const minPlayerSize = 200;
const minPlayerSpace = cellSize.w;
function drawPlayers() {
	if (waitLoad(drawPlayers, this, arguments)) return;
	if (!Players) return;
	if (!playerContent) return;

	var nb = 0;
	for (var i in Players) {
		var playerContener = document.getElementById('player_' + i);
		var node, subnode;
		if (!playerContener) {
			playerContener = document.createElementNS('http://www.w3.org/2000/svg', 'g');
			playerContener.setAttribute("id", 'player_' + i);
			playerContent.appendChild(playerContener);

			node = document.createElementNS('http://www.w3.org/2000/svg', 'rect');
			node.setAttribute("height", cellSize.h * 4);
			node.setAttribute("fill", "#CCC");
			node.setAttribute("stroke", "#999");
			node.setAttribute("rx", cellSize.w / 3.0);
			node.setAttribute("ry", cellSize.w / 3.0);
			playerContener.appendChild(node);

			node = document.createElementNS('http://www.w3.org/2000/svg', 'text');
			node.textContent = "#" + (nb + 1);
			node.setAttribute("font-family", "Verdana");
			node.setAttribute("x", 10);
			node.setAttribute("y", 20);
			playerContener.appendChild(node);

			node = document.createElementNS('http://www.w3.org/2000/svg', 'g');
			node.setAttribute('transform', 'translate(30,45)');
			subnode = document.createElementNS('http://www.w3.org/2000/svg', 'use');
			subnode.setAttribute("href", "#dice");
			subnode.setAttribute("transform", "scale(0.7,0.7)");
			subnode.setAttribute("fill", PlayersColor[i]);
			node.appendChild(subnode);
			subnode = document.createElementNS('http://www.w3.org/2000/svg', 'text');
			subnode.setAttribute("id", "score");
			subnode.textContent = "0";
			subnode.setAttribute("font-size", 25);
			subnode.setAttribute("font-family", "Verdana");
			subnode.setAttribute("font-weight", "bold");
			subnode.setAttribute("x", 25);
			subnode.setAttribute("y", 14);
			node.appendChild(subnode);
			subnode = document.createElementNS('http://www.w3.org/2000/svg', 'use');
			subnode.setAttribute("href", "img/treasure.svg#treasure");
			subnode.setAttribute("transform", "scale(0.35,0.35)");
			subnode.setAttribute("x", 300);
			subnode.setAttribute("fill", PlayersColor[i]);
			node.appendChild(subnode);
			subnode = document.createElementNS('http://www.w3.org/2000/svg', 'text');
			subnode.setAttribute("id", "stock");
			subnode.textContent = "0";
			subnode.setAttribute("font-size", 25);
			subnode.setAttribute("font-family", "Verdana");
			subnode.setAttribute("font-weight", "bold");
			subnode.setAttribute("x", 130);
			subnode.setAttribute("y", 14);
			node.appendChild(subnode);
			playerContener.appendChild(node);

			node = document.createElementNS('http://www.w3.org/2000/svg', 'g');
			node.setAttribute("id", "player_data");
			playerContener.appendChild(node);
		}
		var _w = gameSize.w + cellSize.w / 2.0;
		var _n = Players.length;
		var size = (_w - (_n - 1) * minPlayerSpace) / _n;
		if (minPlayerSize > size) {
			var _n = Math.floor((_w + minPlayerSpace) / (minPlayerSize + minPlayerSpace));
			size = (_w - (_n - 1) * minPlayerSpace) / _n;
		}
		var _x = gameOffset.x + (nb % _n) * (size + minPlayerSpace);
		var _y = gameOffset.y + cellSize.h * 3 + gameSize.h + cellSize.h + Math.floor(nb / _n) * (cellSize.h * 5);
		playerContener.setAttribute('transform', 'translate(' + _x + ',' + _y + ')');
		playerContener.querySelector("rect").setAttribute("width", size);

		var playerData = playerContener.querySelector("#player_data");
		updatePlayerContent(playerContener, Players[i]);

		nb++;
	}
}

function updatePlayerContent(contener, player) {
	if (!contener instanceof SVGElement) contener = document.getElementById('player_' + contener);
	if (!contener) return;

	var node;

	// Set text
	if (!(node = contener.querySelector("#name"))) {
		node = document.createElementNS('http://www.w3.org/2000/svg', 'text');
		node.setAttribute('id', 'name');
		node.setAttribute("font-family", "Verdana");
		node.setAttribute('font-style', 'italic');
		node.setAttribute("font-weight", "bold");
		node.setAttribute('x', '40');
		node.setAttribute('y', '20');
		node.setAttribute('lengthAdjust', 'spacingAndGlyphs');
		contener.appendChild(node);
	}
	node.textContent = player.name;
}

var GameStates = [];
var CurGameState = 0;
function setState(gameState) {
	var turn = parseInt(gameState.id);
	GameStates[turn] = gameState;
	if (CurGameState <= turn) {
		displayState(turn);
	}
}

var CurrentStateChange = false;
var NextStateChange = [];
async function displayState(turn, details = true) {
	if (waitLoad(displayState, this, arguments)) return;
	NextStateChange.push({ "turn": turn, "details": details });
	if (CurrentStateChange) return;
	CurrentStateChange = true;

	do {
		var args = NextStateChange.shift();
		await _displayState(args["turn"], args["details"]);
	} while (NextStateChange.length);
	CurrentStateChange = false;
}

async function _displayState(turn, details) {
	if (!infoContent) return;

	var newGameState = GameStates[turn];
	var oldGameState = turn != CurGameState ? GameStates[CurGameState] : null;
	if (!newGameState) return;

	if (details && newGameState.turn.player) {
		infoContent.querySelector("#text").textContent = "Joueur #" + (newGameState.turn.player + 1);
		var diceContent = infoContent.querySelector("#dices");
		var node;
		while (node = diceContent.firstChild) node.remove();
		var offset = 0;
		var player = [newGameState.turn.attack.from, newGameState.turn.attack.to];
		var region;
		for (var p = 0; p < 2; ++p) {
			var score = 0;
			for (var i in newGameState.turn.dices[p]) {
				var dice = newGameState.turn.dices[p][i];
				if (dice > 0) {
					score += dice;
					var subnode = document.createElementNS('http://www.w3.org/2000/svg', 'use');
					subnode.setAttribute("href", "#dicef" + dice);
					subnode.setAttribute("transform", "translate(" + offset + ",0) scale(1,1)");
					subnode.setAttribute("fill", PlayersColor[oldGameState.map[player[p]].owner]);
					diceContent.appendChild(subnode);
					offset += 35;
				}
			}
			var subnode = document.createElementNS('http://www.w3.org/2000/svg', 'text');
			subnode.setAttribute("font-size", "18");
			subnode.setAttribute("font-family", "Verdana");
			subnode.setAttribute('font-style', 'italic');
			subnode.setAttribute("transform", "translate(" + (offset + 5) + ",20)");
			subnode.setAttribute("fill", PlayersColor[oldGameState.map[player[p]].owner]);
			subnode.textContent = score;
			diceContent.appendChild(subnode);
			offset += 60;
			region = document.getElementById("region_" + player[p]);
			if (region) region.querySelector("#anim").beginElement();
		}
		await sleep(1500);
	}
	else {
		infoContent.querySelector("#text").textContent = "";
		var diceContent = infoContent.querySelector("#dices");
		var node;
		while (node = diceContent.firstChild) node.remove();

	}

	for (var i in newGameState.stock) {
		var playerContener = document.getElementById('player_' + i);
		if (!playerContener) continue;
		if (!oldGameState || (newGameState.points[i] != oldGameState.points[i])) {
			var scoreObj = playerContener.querySelector("#score");
			scoreObj.textContent = newGameState.points[i];
		}
		if (!oldGameState || (newGameState.stock[i] != oldGameState.stock[i])) {
			var stockObj = playerContener.querySelector("#stock");
			stockObj.textContent = newGameState.stock[i];
		}
	}

	for (var i in newGameState.map) {
		if (!oldGameState || (newGameState.map[i].dices != oldGameState.map[i].dices) || (newGameState.map[i].owner != oldGameState.map[i].owner)) {
			setRegionData(i, PlayersColor[newGameState.map[i].owner], newGameState.map[i].dices);
		}
	}
	CurGameState = turn;
}

function sleep(ms) {
	return new Promise(resolve => setTimeout(resolve, ms));
}
