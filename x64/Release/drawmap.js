

var ninety_deg = 1.5708;

// configuration
var height_threshold = 3;

var last_response = {status: "failure"};
var canvas = document.getElementById('canvas');

// resize the canvas to fill browser window dynamically
window.addEventListener('resize', resizeCanvas, false);
var half_radius = Math.min(canvas.width/2, canvas.height/2);
var marker_sz = 0.03 * half_radius;

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
    marker_sz = 0.03 * half_radius;
}


function draw_vehicle(ctx) {
    ctx.save();
    ctx.beginPath();
    ctx.rect(-8.072255, -11.726016, 16.382044, 23.397657);
    ctx.fill();
    ctx.stroke();

    ctx.beginPath();
    ctx.rect(-12.700962, -14.927192, 4.536864, 29.847828);
    ctx.fill();
    ctx.stroke();

    ctx.beginPath();
    ctx.rect(8.406098, -14.893661, 4.536864, 29.847828);
    ctx.fill();
    ctx.stroke();
    ctx.restore();
}

function draw_unknown(ctx) {
    ctx.save();
    ctx.beginPath();
    ctx.arc(0.0, 0.0, 17.285156, 0.000000, 6.28318531, 1);
    ctx.fill();
    ctx.stroke();
    ctx.restore();
}

function draw_medic(ctx) {
    ctx.save();
    ctx.beginPath();
    ctx.moveTo(-13.497000, -4.063915);
    ctx.lineTo(-4.219653, -4.161575);
    ctx.lineTo(-4.219653, -13.438916);
    ctx.lineTo(3.885816, -13.438916);
    ctx.lineTo(4.081128, -3.966263);
    ctx.lineTo(13.456125, -4.063923);
    ctx.lineTo(13.553785, 3.455608);
    ctx.lineTo(4.276444, 3.455608);
    ctx.lineTo(4.276444, 13.514205);
    ctx.lineTo(-4.219650, 13.514205);
    ctx.lineTo(-4.219650, 3.357952);
    ctx.lineTo(-13.496997, 3.357952);
    ctx.lineTo(-13.497000, -4.063915);
    ctx.fill();
    ctx.stroke();
    ctx.restore();
}

function draw_assault(ctx) {
    ctx.save();
    ctx.beginPath();
    ctx.moveTo(-1.805455, 14.041255);
    ctx.lineTo(-4.148172, 13.646955);
    ctx.lineTo(-5.936036, 12.266655);
    ctx.lineTo(-9.326814, 9.637451);
    ctx.lineTo(-12.039438, 5.759551);
    ctx.lineTo(-8.155451, 5.627951);
    ctx.lineTo(-13.642354, -2.456649);
    ctx.lineTo(-6.984097, 0.041151);
    ctx.lineTo(-9.758366, -10.409745);
    ctx.lineTo(-2.545258, -2.719649);
    ctx.lineTo(-0.202540, -14.945145);
    ctx.lineTo(2.325136, -2.916749);
    ctx.lineTo(9.846492, -10.409745);
    ctx.lineTo(6.640664, -0.221749);
    ctx.lineTo(13.545520, -2.982449);
    ctx.lineTo(8.058629, 4.707751);
    ctx.lineTo(7.318820, 5.627951);
    ctx.lineTo(12.435813, 5.562951);
    ctx.lineTo(10.647950, 8.389251);
    ctx.lineTo(8.613483, 11.149854);
    ctx.lineTo(4.544551, 13.384655);
    ctx.lineTo(1.215418, 13.910555);
    ctx.lineTo(0.968823, 8.652151);
    ctx.lineTo(2.941636, 6.943151);
    ctx.lineTo(3.126593, 4.182651);
    ctx.lineTo(2.879990, 2.736651);
    ctx.lineTo(0.044064, 2.145051);
    ctx.lineTo(-2.175351, 2.999451);
    ctx.bezierCurveTo(-2.175351, 2.999451, -2.915164, 5.102851, -2.915164, 5.497251);
    ctx.bezierCurveTo(-2.915164, 5.891551, -2.545258, 7.337651, -2.298654, 7.600551);
    ctx.bezierCurveTo(-2.052060, 7.863451, -1.373903, 8.520751, -1.373903, 8.520751);
    ctx.lineTo(-0.880697, 12.661755);
    ctx.lineTo(-1.805455, 14.041255);
    ctx.fill();
    ctx.stroke();
    ctx.restore();
}

function draw_scout(ctx) {
    ctx.save();

    ctx.beginPath();
    ctx.moveTo(0.910677, -9.307873);
    ctx.lineTo(-1.437139, -9.307873);
    ctx.lineTo(-1.575246, -11.344949);
    ctx.lineTo(-2.921787, -11.344949);
    ctx.lineTo(-4.337382, -10.827048);
    ctx.lineTo(-6.374457, -9.653140);
    ctx.lineTo(-7.962685, -8.341125);
    ctx.lineTo(-9.827127, -6.131416);
    ctx.lineTo(-11.001035, -3.610967);
    ctx.bezierCurveTo(-11.001035, -3.610967, -11.035565, -1.539365, -11.035565, -1.401259);
    ctx.bezierCurveTo(-11.035565, -1.263152, -10.862932, -0.848831, -10.862932, -0.848831);
    ctx.lineTo(-1.437142, -1.021465);
    ctx.lineTo(-0.090601, -2.747800);
    ctx.lineTo(1.290467, -0.986938);
    ctx.lineTo(10.888888, -1.055988);
    ctx.lineTo(11.234155, -2.057262);
    ctx.lineTo(11.026995, -3.127590);
    ctx.lineTo(9.887615, -5.889725);
    ctx.lineTo(8.644654, -7.650587);
    ctx.lineTo(6.883792, -9.411449);
    ctx.lineTo(5.330091, -10.481777);
    ctx.lineTo(3.500176, -11.241365);
    ctx.lineTo(1.601208, -11.344945);
    ctx.lineTo(1.117834, -11.275895);
    ctx.lineTo(1.152364, -14.210665);
    ctx.lineTo(3.741866, -14.176135);
    ctx.bezierCurveTo(3.741866, -14.176135, 6.020628, -13.382021, 6.158735, -13.278441);
    ctx.bezierCurveTo(6.296842, -13.174861, 8.644657, -11.655686, 8.644657, -11.655686);
    ctx.lineTo(10.750785, -9.445976);
    ctx.lineTo(12.615226, -6.476680);
    ctx.lineTo(13.823661, -2.437057);
    ctx.lineTo(13.789131, 0.739400);
    ctx.lineTo(12.925964, 4.123016);
    ctx.lineTo(10.957942, 7.851900);
    ctx.lineTo(8.023174, 10.683088);
    ctx.lineTo(3.638283, 12.858270);
    ctx.lineTo(1.808368, 13.341644);
    ctx.lineTo(1.808368, 10.821195);
    ctx.lineTo(4.190710, 10.337821);
    ctx.lineTo(6.434945, 9.025807);
    ctx.lineTo(8.333914, 7.299472);
    ctx.lineTo(9.991194, 5.158817);
    ctx.lineTo(10.992469, 2.741948);
    ctx.lineTo(10.681728, 1.533514);
    ctx.lineTo(1.048780, 1.740674);
    ctx.lineTo(1.014250, 10.579508);
    ctx.lineTo(1.601204, 10.959301);
    ctx.lineTo(1.566674, 13.721437);
    ctx.lineTo(-1.816943, 13.790487);
    ctx.lineTo(-1.816943, 11.270038);
    ctx.lineTo(-1.022829, 10.579504);
    ctx.lineTo(-1.195462, 1.913303);
    ctx.lineTo(-10.897465, 1.982353);
    ctx.lineTo(-10.862935, 4.157535);
    ctx.lineTo(-9.343759, 6.712511);
    ctx.lineTo(-7.686477, 8.369792);
    ctx.lineTo(-5.511295, 9.681807);
    ctx.lineTo(-3.508747, 10.855713);
    ctx.lineTo(-2.127679, 11.166454);
    ctx.lineTo(-2.127679, 13.790483);
    ctx.lineTo(-5.925616, 12.478468);
    ctx.lineTo(-9.689026, 10.130654);
    ctx.lineTo(-12.244003, 7.092304);
    ctx.lineTo(-13.417910, 3.846795);
    ctx.lineTo(-13.728651, 0.014331);
    ctx.lineTo(-13.417910, -4.646773);
    ctx.lineTo(-11.208202, -8.582817);
    ctx.lineTo(-8.377011, -11.448534);
    ctx.lineTo(-4.751708, -13.520136);
    ctx.lineTo(-1.160931, -14.141616);
    ctx.lineTo(0.772564, -14.038036);
    ctx.lineTo(0.910677, -9.307873);
    ctx.stroke();
    ctx.fill();
    ctx.restore();
}
function draw_support(ctx) {
    ctx.save();
    ctx.transform(1.000000, 0.000000, 0.000000, 1.000000, -54.485662, -38.460582);

    ctx.beginPath();

    ctx.moveTo(44.815655, 35.481880);
    ctx.lineTo(44.884705, 48.774659);
    ctx.lineTo(49.580336, 48.774659);
    ctx.lineTo(49.649386, 35.240193);
    ctx.lineTo(49.166013, 30.130242);
    ctx.lineTo(47.267044, 27.471686);
    ctx.lineTo(45.644289, 30.061189);
    ctx.lineTo(44.884702, 35.723567);
    ctx.lineTo(44.815655, 35.481880);
    ctx.fill();
    ctx.stroke();

    ctx.beginPath();
    ctx.moveTo(51.513834, 35.654514);
    ctx.lineTo(51.479304, 51.605848);
    ctx.lineTo(57.383370, 51.571318);
    ctx.lineTo(57.452420, 35.585456);
    ctx.lineTo(56.865466, 30.199291);
    ctx.lineTo(55.726085, 27.195469);
    ctx.lineTo(54.724811, 25.538187);
    ctx.lineTo(53.343743, 26.884728);
    ctx.lineTo(52.411522, 30.061185);
    ctx.lineTo(51.513834, 35.654514);
    ctx.fill();
    ctx.stroke();

    ctx.beginPath();
    ctx.moveTo(58.937074, 35.447354);
    ctx.lineTo(59.075181, 48.774659);
    ctx.lineTo(64.047026, 48.740129);
    ctx.lineTo(64.185133, 35.447351);
    ctx.lineTo(63.391018, 30.302872);
    ctx.lineTo(62.286164, 28.196744);
    ctx.lineTo(61.422997, 27.126416);
    ctx.lineTo(60.697936, 28.196744);
    ctx.lineTo(60.076455, 30.130239);
    ctx.lineTo(58.937074, 35.447354);
    ctx.fill();
    ctx.stroke();
    ctx.restore();
}

function draw_arrow(ctx, rotation) {
    ctx.save();
    ctx.rotate(rotation);
    ctx.beginPath();
    ctx.fillStyle = 'rgb(200, 200, 200)';

    ctx.moveTo(-11.718750, -14.337020);
    ctx.lineTo(-9.863281, -15.899520);
    ctx.lineTo(-7.031250, -17.266707);
    ctx.lineTo(-4.101563, -18.047957);
    ctx.lineTo(-0.781250, -18.438582);
    ctx.lineTo(3.417969, -18.438582);
    ctx.lineTo(6.542969, -17.462020);
    ctx.lineTo(10.839844, -15.118270);
    ctx.lineTo(12.500000, -13.946395);
    ctx.lineTo(0.097656, -40.606551);
    ctx.lineTo(-11.718750, -14.337020);
    ctx.fill();
    ctx.restore();
}

function draw_player(ctx, player, pos, color) {
    ctx.save();

    ctx.strokeStyle = color;
    ctx.fillStyle = (Math.abs(pos.y) <= height_threshold) ? color : "#333";

    ctx.translate(pos.x, pos.z);
    if(player.isVehicle === true ) {
        draw_vehicle(ctx);
    } else {
        draw_arrow(ctx, pos.yaw);
        switch( player.playerClass ) {
            case "Assault": draw_assault(ctx); break;
            case "Medic":   draw_medic(ctx); break;
            case "Scout":   draw_scout(ctx); break;
            case "Support": draw_support(ctx); break;
            default: draw_unknown(ctx); break;
        }
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
        yaw: player.yaw - local_player_yaw// - Math.PI
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
    for(var i = increment; i < map_radius; i += increment ) {
        draw_ring(ctx, i);
    }
}

function draw_flag(ctx, pos, color, neutral, num_defenders, percent) {
    ctx.save();
    ctx.translate(pos.x,pos.z);

    var width = marker_sz  * 3;
    var height = marker_sz * 3;
    var grd = ctx.createLinearGradient(0, width/2, 0, -width/2);

    if( percent < 0.99 ) {
        grd.addColorStop(percent, color);
        grd.addColorStop(percent, neutral);
        grd.addColorStop(1, neutral);
    } else {
        grd.addColorStop(1, color);
    }


    ctx.strokeStyle = grd;
    ctx.fillStyle = grd;
    ctx.lineWidth = 2;

    ctx.beginPath();
    ctx.moveTo(0, height/2);
    ctx.lineTo(-width/2, 0);
    ctx.lineTo(-width/4, 0);
    ctx.lineTo(-width/4, -height/2);
    ctx.lineTo(width/4, -height/2);
    ctx.lineTo(width/4, 0);
    ctx.lineTo(width/2, 0);
    ctx.lineTo(0, height/2);
    ctx.fill();

    ctx.font = "32px Arial";
    ctx.fillStyle = color;
    ctx.textAlign = "center";
    ctx.fillText(num_defenders,0,-2*height/3);
    ctx.restore();
}

function draw_team(ctx, local, team_array, color ) {
    if (team_array == null ) return;
    var local_pos = { x: local.x, y: local.y, z:local.z }
    for (var i = 0; i < team_array.length; i++) {
        var remote = team_array[i];
        if (remote.health === 0.0 ) continue;

        var remote_pos = {x: remote.x, y: remote.y, z: remote.z, yaw: remote.yaw};
        var player_space = world_to_player(local_pos, remote_pos);
        var playerup_space = player_to_playerup(local.yaw, player_space);
        var map_space = playerup_to_map(playerup_space);
        draw_player(ctx, remote, map_space, color);
    }
}

function draw_flags(ctx, local, flag_array, mate, enemy, neutral) {
    if (flag_array == null ) return;
    var local_pos = { x: local.x, y: local.y, z:local.z }
    for(var i = 0; i < flag_array.length; i++) {
        var flag = flag_array[i];

        var flag_pos = {x: flag.x, y: flag.y, z: flag.z };
        var player_space = world_to_player(local_pos, flag_pos);
        var playerup_space = player_to_playerup(local.yaw, player_space);
        var map_space = playerup_to_map(playerup_space);

        if (flag.OwnerTeam == "0" ) {
            draw_flag(ctx, map_space, neutral, neutral, flag.Defenders, flag.OwnerCapturePercent);
        } else if (flag.OwnerTeam == local.team) {
            draw_flag(ctx, map_space, mate, neutral, flag.Defenders, flag.OwnerCapturePercent);
        } else {
            draw_flag(ctx, map_space, enemy, neutral, flag.Defenders, flag.OwnerCapturePercent);
        }
    }
}

function draw_map() {
    if (last_response.status !== "success" ) {
        return;
    }
    resizeCanvas();
    var ctx = canvas.getContext('2d');
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    ctx.save();

    ctx.translate(canvas.width/2, canvas.height/2);
    var enemy_color = '#df3939';
    var mate_color = '#52a5f8';
    var neutral_color = "rgba(200,200,200,1)";
    var unknown_color = '#ffffff';


    draw_range_rings(ctx);

    var fov_rad = (Math.PI/180)*last_response.local_player.fov;
    var center_screen_fov = fov_rad * 0.5;
    var under_reticle_fov = fov_rad * 0.2;
    draw_fov_cone(ctx, center_screen_fov, 0.15);
    draw_fov_cone(ctx, under_reticle_fov, 0.15);
    draw_flags(ctx, last_response.local_player, last_response.flags, mate_color, enemy_color, neutral_color);
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

resizeCanvas();
setInterval(request_data, 33);
