open Printf
exception Failure of string

let filename    = "out.ppm"
let width       = 800
let height      = 600
let point_count = 20
let color_bg    = 0x0
let color_point = 0xffffff

let colors = [|
    0xfb4934;
    0xb8bb26;
    0xfabd2f;
    0x83a598;
    0xd3869b;
    0x8ec07c;
    0xfe8019;

    0x1d2021;
    0x282828;
    0x32302f;
    0x3c3836;
    0x504945;
    0x665c54;
    0x7c6f64;
    0x7c6f64;
    |]








type point = { x : int; y : int }
type grid = int array array

let print_point (point : point) =
    printf "x: %d, y: %d\n" point.x point.y






let write_pixel (chan : out_channel) (pixel : int) =
    output_byte chan (0xff land (Int.shift_right pixel (8*2)));
    output_byte chan (0xff land (Int.shift_right pixel (8*1)));
    output_byte chan (0xff land (Int.shift_right pixel (8*0)));
    ()

let write_canvas_to_file (canvas : grid) =
    let file : out_channel = open_out_bin filename in
    fprintf file "P6\n%d %d\n255\n" width height;

    let for_each_column col = Array.map (fun pixel -> write_pixel file pixel) col in
    Array.map (fun col -> for_each_column col) canvas |> ignore;

    close_out file;
    ()

let canvas_init () : grid =
    Array.make_matrix height width color_bg

let generate_points (count : int) : point list =
    Random.self_init ();

    let rec inner list i =
        if i == count then list else
            let x = Random.int width in
            let y = Random.int height in
            let p : point = { x; y } in
            inner (p :: list) (i+1)

    in inner [] 0


let calculate_distance (p : point) : float =
    let x = (float p.x) ** 2.0 in
    let y = (float p.y) ** 2.0 in
    sqrt (x +. y)

let point_sub (p1 : point) (p2 : point) : point =
    { x = p1.x - p2.x; y = p1.y - p2.y }

let get_min_distance (dists : float list) : float =
    let rec inner x list =
        match list with
        | head :: tail -> inner (if head < x || x == -1.0 then head else x) tail
        | [] -> x
    in inner ~-.1.0 dists

let index_of_list (query : float) (list : float list) : int =
    let rec inner list index =
        match list with
        | head :: tail -> if head == query then index else (inner tail index+1)
        | [] -> raise (Failure "Item not found")
    in inner list 0


let get_nearest_point (point : point) (points : point list) : point * int =
    let distances = List.map (fun p -> point_sub p point |> calculate_distance) points in
    let min_dist = get_min_distance distances in
    let index = index_of_list min_dist distances in
    List.nth points index, index





let render_circle (p : point) (canvas : grid) =
    canvas.(p.y  ).(p.x  ) <- color_point;
    canvas.(p.y+1).(p.x  ) <- color_point;
    canvas.(p.y  ).(p.x+1) <- color_point;
    canvas.(p.y-1).(p.x  ) <- color_point;
    canvas.(p.y  ).(p.x-1) <- color_point;
    ()


let render_points (canvas : grid) (points : point list) =
    ignore (List.map (fun p -> render_circle p canvas) points)


let render (canvas : grid) (points : point list) =
    for y = 0 to height-1 do
        for x = 0 to width-1 do
            let (nearest, index) : point * int = get_nearest_point { x; y } points in
            (* canvas.(y).(x) <- int_of_float (calculate_distance nearest); *)
            (* canvas.(y).(x) <- nearest.x * nearest.y; *)
            canvas.(y).(x) <- colors.(index mod (Array.length colors));
            ()
        done
    done



let () =
    let canvas : grid = canvas_init () in
    let points : point list = generate_points point_count in
    ignore (List.map (fun x -> print_point x) points);

    render canvas points;
    render_points canvas points;

    write_canvas_to_file canvas;
    ()
