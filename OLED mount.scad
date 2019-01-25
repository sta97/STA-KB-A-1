display_hole_distance = 28.956;
pcb_hole_positions = [[0,10,0],[-15,-10,0],[15,-10,0]];
pcb_mount_size = [[0,15],[-20,-12.5],[20,-12.5]];
pcb_mount_thickness = [0,0,2];

pcb_mount();
translate([0,0,30]) display_mount();
attachment_curve();

module attachment_curve() {
     for(n = [0:100]) {
         t = n/100;
         translate([sin(t*180)*-20-18+t*3,t*-8,cos(t*180)*-15.5+15.5]) rotate([-45*t,180*t,30*t]) cube([2,25,2],true);
     }   
}

module pcb_mount() {
    difference(){
    cube([36,26,2],true);
       pcb_holes();
        cube([15,10,2.5],true);
    }
}

module pcb_holes() {
    translate(pcb_hole_positions[0]) hole();
    translate(pcb_hole_positions[1]) hole();
    translate(pcb_hole_positions[2]) hole();
}

module display_mount() {
    rotate([45,0,30]){
difference(){
cube([display_hole_distance+6,display_hole_distance+6,2],true);
    translate([0,display_hole_distance/2,0]) 
    cube([display_hole_distance-6,7,2.5],true);  
display_holes();
}
}
}

module display_holes() {
translate([display_hole_distance/-2,display_hole_distance/-2,0]){
hole();
translate([display_hole_distance, 0, 0]) {
    hole();
translate([0, display_hole_distance, 0]) {
    hole();
}
}
translate([0, display_hole_distance, 0]) {
    hole();
}
}
}

module hole() {
cylinder(h = 2.5, d = 2, center = true, $fn = 100);
}