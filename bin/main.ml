open Printf
open Raylib

exception Failure of string

let filename    = "out.ppm"
let width       = 800
let height      = 600
(* let width       = 1920 *)
(* let height      = 1080 *)
let point_count = 10
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
        if i = count then list else
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
        | head :: tail -> inner (if head < x || x = -1.0 then head else x) tail
        | [] -> x
    in inner ~-.1.0 dists

let index_of_list (query : float) (list : float list) : int =
    match List.find_index (fun x -> x = query) list with
    | Some index -> index
    | None -> raise (Failure "Item not found")

let get_nearest_point (point : point) (points : point list) : point * int =
    let distances = List.map (fun p -> point_sub p point |> calculate_distance) points in
    let min_dist = get_min_distance distances in
    let index = index_of_list min_dist distances in
    List.nth points index, index




let render_circle (p : point) (canvas : grid) =
    let get_x x = if x < 0 then 0 else if x >= width then width-1 else x in
    let get_y y = if y < 0 then 0 else if y >= height then height-1 else y in
    let set x y = canvas.(get_y (p.y+y)).(get_x (p.x+x)) <- color_point in
    set 0 0;
    set 1 0;
    set 0 1;
    set (-1) 0;
    set 0 (-1);
    ()


let render_points (canvas : grid) (points : point list) =
    ignore (List.map (fun p -> render_circle p canvas) points)


let render (canvas : grid) (points : point list) =
    for y = 0 to height-1 do
        for x = 0 to width-1 do
            let (_nearest, index) : point * int = get_nearest_point { x; y } points in
            (* canvas.(y).(x) <- int_of_float (calculate_distance nearest); *)
            (* canvas.(y).(x) <- nearest.x * nearest.y; *)
            canvas.(y).(x) <- colors.(index mod (Array.length colors));
            ()
        done
    done

let pixel_to_color pixel = 
    let r = (0xff land (Int.shift_right pixel (8*2))) in
    let g = (0xff land (Int.shift_right pixel (8*1))) in
    let b = (0xff land (Int.shift_right pixel (8*0))) in
    Color.create r g b 255

let draw_canvas_on_screen (canvas : grid) =
    let rect_size = 1 in

    for y = 0 to height-1 do
        for x = 0 to width-1 do
            let pixel : int = canvas.(y).(x) in
            let color = pixel_to_color pixel in
            draw_rectangle x y rect_size rect_size color;
            ()
        done;
    done;
    ()



let setup () =
    Raylib.init_window width height "Voronoi";
    Raylib.set_target_fps 60

let rec loop (canvas : grid) =
    if Raylib.window_should_close () then Raylib.close_window ()
    else
        begin_drawing ();
        clear_background Color.black;
        draw_canvas_on_screen canvas;
        end_drawing ();
        loop canvas




let () =
    let canvas : grid = canvas_init () in
    let points : point list = generate_points point_count in
    render canvas points;
    render_points canvas points;

    setup () |> loop canvas
    (* write_canvas_to_file canvas; *)
    ()
