 // Vector2f color_point;
    // color_point.x = (p3.y - y_coord) * cu.x + (y_coord - p1.y) * cl.x;
    // color_point.y = (p3.y - y_coord) * cu.y + (y_coord - p1.y) * cl.y;
//printf("%f %f\n",color_point.x,color_point.y);


// float x_round_down = floor(x_coord);
//     float y_round_down = floor(y_coord);
//     float x_round_up = ceil(x_coord);
//     float y_round_up = ceil( y_coord);

//     Vector2f p1 = Vector2f(x_round_down,y_round_down);
//     Vector2f p2 = Vector2f(x_round_up,y_round_down);
//     Vector2f p3 = Vector2f(x_round_down,y_round_up);
//     Vector2f p4 = Vector2f(x_round_up,y_round_up);

//     Vector3f c1 = loadPixelColor(p1.x,p1.y);
//     Vector3f c2 = loadPixelColor(p2.x,p2.y);
//     Vector3f c3 = loadPixelColor(p3.x,p3.y);
//     Vector3f c4 = loadPixelColor(p4.x,p4.y);

//     float factor_1 = x_coord - x_round_down;
//     float factor_2 = y_coord - y_round_down;


//     Vector3f inter_col = (1-factor_1)*(1-factor_2)*c1 + factor_1*(1-factor_2)*c2 + (1-factor_1)*factor_2*c3 + factor_1*factor_2*c4;
//     return inter_col;

   // return loadPixelColor(color_point.x,color_point.y);