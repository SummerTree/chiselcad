// Tier F visual test — v3 Phase 3: polyhedron(), resize(), nested extrusion
// Open in ChiselCAD (and optionally OpenSCAD) to compare results.

// --- Scene 1: polyhedron() — a tetrahedron (all-triangle faces)
translate([0, 0, 0])
    polyhedron(
        points = [[0,0,0], [10,0,0], [5,10,0], [5,5,10]],
        faces  = [[0,1,2], [0,1,3], [1,2,3], [0,2,3]]
    );

// --- Scene 2: polyhedron() — a square pyramid (one quad face, fan-triangulated)
translate([20, 0, 0])
    polyhedron(
        points = [[0,0,0], [8,0,0], [8,8,0], [0,8,0], [4,4,8]],
        faces  = [[0,1,2,3], [0,1,4], [1,2,4], [2,3,4], [3,0,4]]
    );

// --- Scene 3: polyhedron() — a hand-built cube (six quad faces)
translate([40, 0, 0])
    polyhedron(
        points = [
            [0,0,0], [6,0,0], [6,6,0], [0,6,0],
            [0,0,6], [6,0,6], [6,6,6], [0,6,6]
        ],
        faces = [
            [0,1,2,3], // bottom
            [4,5,6,7], // top
            [0,1,5,4], // front
            [1,2,6,5], // right
            [2,3,7,6], // back
            [3,0,4,7]  // left
        ]
    );

// --- Scene 4: resize() — uniform newsize on a sphere
translate([0, 25, 0])
    resize([10, 10, 10])
        sphere(r = 2, $fn = 32);

// --- Scene 5: resize() — stretch only the x axis (y/z stay at 0 = unchanged)
translate([20, 25, 0])
    resize([16, 0, 0])
        cube([4, 4, 4], center = true);

// --- Scene 6: resize() — auto= proportionally scales the zeroed axis
translate([40, 25, 0])
    resize([12, 0, 0], auto = true)
        cylinder(h = 4, r = 2, $fn = 32);

// --- Scene 7: nested linear_extrude — outer extrude re-extrudes the
// silhouette of a fully realized inner extrusion
translate([0, 50, 0])
    linear_extrude(height = 6)
        linear_extrude(height = 3)
            square([6, 6]);

// --- Scene 8: nested extrusion mixing kinds — linear_extrude wrapping a
// rotate_extrude (torus silhouette re-extruded into a flat-topped solid)
translate([25, 50, 0])
    linear_extrude(height = 4)
        rotate_extrude($fn = 32)
            translate([5, 0, 0])
                circle(r = 2, $fn = 24);
