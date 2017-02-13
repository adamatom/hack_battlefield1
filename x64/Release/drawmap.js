

var ninety_deg = 1.5708;

// configuration
//var cone_depth = 80; // 80 meters to help with sniping
var height_threshold = 3;
//var max_map_radius = 80;
var canvas = document.getElementById('canvas');

var last_response = {status: "failure"};
// resize the canvas to fill browser window dynamically
window.addEventListener('resize', resizeCanvas, false);
var half_radius = Math.min(canvas.width/2, canvas.height/2);
var marker_sz = 0.04 * half_radius;

function get_dom_int( element, otherwise ) {
    var dom_item = document.getElementById(element);
    if (dom_item == null) return otherwise;
    var dom_value = dom_item.value;
    if ( isNaN(dom_value) ) return otherwise;
    return parseInt(dom_value);
}

function resizeCanvas() {
    canvas.width = window.innerWidth;
    canvas.height = window.innerHeight;
    half_radius = Math.min(canvas.width/2, canvas.height/2);
    marker_sz = 0.04 * half_radius;
}

function draw_vehicle(ctx, pos, color) {
    ctx.save();
    ctx.translate(pos.x,pos.z);
    ctx.fillStyle = color;
    ctx.strokeStyle = ctx.fillStyle;
    ctx.beginPath();
    // "tracks"
    var track_width = marker_sz * 0.3;
    var track_height = marker_sz * 1.5;
    ctx.fillRect(-0.5*marker_sz-track_width/2, -0.5*track_height, track_width, track_height);
    ctx.fillRect(0.5*marker_sz-track_width/2, -0.5*track_height, track_width, track_height);
    // body
    if( Math.abs(pos.y) <= height_threshold ) {
        ctx.fillRect(-0.5*marker_sz, -0.5*marker_sz, marker_sz, marker_sz);
    } else {
        ctx.rect(-0.5*marker_sz, -0.5*marker_sz, marker_sz, marker_sz);
        ctx.stroke();
    }
    ctx.closePath();
    ctx.restore();
}

function draw_soldier(ctx, pos, color) {
    ctx.save();
    ctx.fillStyle = color;
    ctx.strokeStyle = ctx.fillStyle;
    //ctx.beginPath();

    ctx.translate(pos.x,pos.z);
    ctx.rotate(pos.yaw);

    var halfm = marker_sz / 2;
    var height_diff_large = Math.abs(pos.y) <= height_threshold;

    ctx.beginPath();
    ctx.arc(0,0, (marker_sz-(marker_sz/4))/2,0, 2*Math.PI);
    if( height_diff_large ) {
        ctx.fill();
    } else {
        ctx.stroke();
    }

    ctx.beginPath();
    ctx.moveTo(-halfm, halfm);
    ctx.lineTo(halfm,halfm);
    ctx.lineTo(0, marker_sz);
    ctx.lineTo(-halfm, halfm);

    if( height_diff_large ) {
        ctx.fill();
    } else {
        ctx.stroke();
    }

    ctx.restore();
}

// convert from world to player space (translation)
// then from player space to player-up space (rotation)
// then from player-up space to map space (scale)

function world_to_player(local, remote) {
    return {
        x: remote.x - local.x,
        y: remote.y - local.y,
        z: remote.z - local.z,
        yaw: remote.yaw
    }
}

function player_to_playerup(local_player_yaw, player) {
    return {
        // yaw is angle from z axis, which points south. x points east
        x: -1*player.x*Math.cos(local_player_yaw) - player.z*Math.sin(local_player_yaw),
        y: player.y,
        z: player.x*Math.sin(local_player_yaw) - player.z*Math.cos(local_player_yaw),
        yaw: player.yaw - local_player_yaw - Math.PI
    }
}

function playerup_dist_to_map_dist( dist ) {
    var max_map_radius = get_dom_int('map_radius', 80);
    return (dist / max_map_radius) * half_radius;
}

function playerup_to_map(player) {
    var max_map_radius = get_dom_int('map_radius', 80);
    var norm_x = player.x / max_map_radius;
    var norm_z = player.z / max_map_radius;
    return {
        x: norm_x*half_radius,
        y: player.y,
        z: norm_z*half_radius,
        yaw: player.yaw
    }
}

function draw_fov_cone(ctx, fov, alpha) {
    ctx.save();
    var cone_depth = get_dom_int('cone_depth', 25);
    var cone_width = Math.tan(fov/2) * cone_depth;
    // cheat by using one point of the fov triangle to get coordinates, then mirror.
    var map_coords = playerup_to_map({x: cone_width, y: 0, z: 0-cone_depth});
    ctx.beginPath();
    ctx.moveTo(0, 0);
    ctx.lineTo(-map_coords.x, map_coords.z); // mirror about y axis
    ctx.lineTo(map_coords.x, map_coords.z); 
    ctx.lineTo(0, 0);
    ctx.fillStyle = "rgba(200,200,200, "+ alpha + ")";
    ctx.fill();
    ctx.closePath();
    ctx.restore();
}

function draw_ring(ctx, radius) {
    var map_radius = playerup_dist_to_map_dist( radius );
    ctx.save();
    ctx.strokeStyle = "rgba(120, 120, 120, 0.8)";
    ctx.beginPath();
    ctx.arc(0,0, map_radius, 0, 2*Math.PI);
    ctx.stroke();
    ctx.closePath();
    ctx.restore();


}

function draw_range_rings(ctx, increment) {
    var map_radius = get_dom_int('map_radius', 80);
    var increment = get_dom_int('range_increment', 10);
    if( increment <= 0) return;
    for(var i = 0; i < map_radius; i += increment ) {
        draw_ring(ctx, i);
    }
}

function draw_team(ctx, local, team_array, color ) {
    if (team_array == null ) return;
    var local_pos = { x: local.x, y: local.y, z:local.z }
    for (var i = 0, len = team_array.length; i < len; i++) {
        var remote = team_array[i];
        if (remote.health === 0.0 ) continue;

        var remote_pos = {x: remote.x, y: remote.y, z: remote.z, yaw: remote.yaw};
        var player_space = world_to_player(local_pos, remote_pos);
        var playerup_space = player_to_playerup(local.yaw, player_space);
        var map_space = playerup_to_map(playerup_space);
        if( remote.is_vehicle ) {
            draw_vehicle(ctx, map_space, color);
        } else {
            draw_soldier(ctx, map_space, color);
        }
    }
}

function draw_map() {
    resizeCanvas();
    var ctx = canvas.getContext('2d');
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    ctx.save();

    ctx.translate(canvas.width/2, canvas.height/2);
    var enemy_color = '#df3939';
    var mate_color = '#52a5f8';
    var unknown_color = '#ffffff';

    if (last_response.status !== "success" ) { return; }

    draw_range_rings(ctx);

    var fov_rad = (Math.PI/180)*last_response.local_player.fov;
    var center_screen_fov = fov_rad * 0.5;
    var under_reticle_fov = fov_rad * 0.2;
    draw_fov_cone(ctx, center_screen_fov, 0.15); 
    draw_fov_cone(ctx, under_reticle_fov, 0.15); 

    if (last_response.local_player.team === 1 ) {
        draw_team(ctx, last_response.local_player, last_response.team1, mate_color);
        draw_team(ctx, last_response.local_player, last_response.team2, enemy_color);
    } else if (last_response.local_player.team === 2) {
        draw_team(ctx, last_response.local_player, last_response.team2, mate_color);
        draw_team(ctx, last_response.local_player, last_response.team1, enemy_color);
    } else {
        draw_team(ctx, last_response.local_player, last_response.team1, unknown_color);
        draw_team(ctx, last_response.local_player, last_response.team2, unknown_color);
    }

    ctx.restore();
}

function request_data() {
    var xmlhttp = new XMLHttpRequest();
    var url = "/report";

    xmlhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            last_response = JSON.parse(this.responseText);
            draw_map();
        }
    };
    xmlhttp.open("GET", url, true);
    xmlhttp.send();
}

setInterval(request_data, 33);
resizeCanvas();
