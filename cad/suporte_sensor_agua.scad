/*
  Suporte paramétrico para sensor analógico de água.
  DrenaHub — Global Solution Maker Lab
  Ajuste as medidas ao sensor realmente comprado.
*/

sensor_width = 20.0;
sensor_thickness = 1.8;
sensor_length = 62.0;
wall = 2.4;
clearance = 0.6;
holder_height = 24.0;
mount_width = 34.0;
mount_height = 24.0;
mount_thickness = 3.0;
hole_diameter = 3.4;
hole_spacing = 22.0;

module sensor_clip() {
    difference() {
        cube([
            sensor_width + 2 * wall + clearance,
            sensor_thickness + 2 * wall + clearance,
            holder_height
        ]);
        translate([wall, wall, wall])
            cube([
                sensor_width + clearance,
                sensor_thickness + clearance,
                holder_height
            ]);
        translate([
            wall + 4,
            -1,
            wall + 6
        ])
            cube([
                sensor_width - 8 + clearance,
                sensor_thickness + 2 * wall + clearance + 2,
                holder_height
            ]);
    }
}

module mounting_plate() {
    difference() {
        cube([mount_width, mount_thickness, mount_height]);
        for (x = [
            mount_width / 2 - hole_spacing / 2,
            mount_width / 2 + hole_spacing / 2
        ]) {
            translate([x, -1, mount_height / 2])
                rotate([-90, 0, 0])
                    cylinder(h = mount_thickness + 2, d = hole_diameter, $fn = 36);
        }
    }
}

union() {
    mounting_plate();
    translate([
        (mount_width - sensor_width - 2 * wall - clearance) / 2,
        mount_thickness,
        0
    ])
        sensor_clip();
}
