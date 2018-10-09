    float x1 = _pos.x();
    float y1 = _pos.y();
    float laser_range = _lasers[0].get_range();
    float camera_range = _camera.get_range();
    float inc = _camera.get_angular_range() / (_camera.pixels().size() - 1);
    for (size_t i = 0; i < m->get_illuminated_switches().size(); ++i)
    {
      Map::ill_sw_t isv = m->get_illuminated_switches()[i];

      if(isv->get_on())
      {
        float x_isv = isv->get_x();
        float y_isv = isv->get_y();
        float r = isv->get_radius();

        float dist = sqrtf((x_isv - x1) * (x_isv - x1) + (y_isv - y1) * (y_isv - y1));

        if(((dist - r) <= laser_range) || (dist <= camera_range))
        {
          float ang = normalize_angle(asin((y_isv - y1)/dist));

          if(x1 >= x_isv)
          {
            if(y1 >= y_isv)
              ang = -M_PI/2 - (M_PI/2 + ang);
            else
              ang = M_PI/2 + (M_PI/2 - ang);
          }

          float x_isv2 = x_isv + cosf(normalize_angle(ang + M_PI/2)) * r;
          float y_isv2 = y_isv + sinf(normalize_angle(ang + M_PI/2)) * r;
          float dist2 = sqrtf((x_isv2 - x1) * (x_isv2 - x1) + (y_isv2 - y1) * (y_isv2 - y1));
          float limAng1 = normalize_angle(asin((y_isv2 - y1)/dist2));

          if(x1 >= x_isv2)
          {
            if(y1 >= y_isv2)
              limAng1 = -M_PI/2 - (M_PI/2 + limAng1);
            else
              limAng1 = M_PI/2 + (M_PI/2 - limAng1);
          }

          float limAng2 = normalize_angle(ang - (limAng1 - ang));
    
          for (size_t j = 0; j < _lasers.size(); ++j)
          {
            if((vec_dist[j] < 0.0f) || (dist < vec_dist[j]))
            {
              float angle = normalize_angle(_lasers[j].get_angle() + _pos.theta());

              if(limAng2 < limAng1)
              {
                if(angle >= limAng2 && angle <= limAng1)
                  vec_dist[j] = dist - r;
              }
              else
              {
                if((angle >= limAng2 && angle <= M_PI) || (angle <= limAng1 && angle >= -M_PI))
                  vec_dist[j] = dist - r;
              }
            }
          }
    
          float ro = -_camera.get_angular_range() / 2.0f;
          for (size_t j = 0; j < _camera.pixels().size(); ++j, ro += inc)
          {
            assert(j < _camera.pixels().size());

            if((_camera.dist()[j] < 0.0f) || (dist < _camera.dist()[j]))
            {
              float angle = normalize_angle(ro + _pos.theta());

              if(limAng2 < limAng1)
              {
                if(angle >= limAng2 && angle <= limAng1)
                {
                  _camera.set_pixel(j, isv->get_color());
                  _camera.set_dist(j, dist);
                }
              }
              else
              {
                if((angle >= limAng2 && angle <= M_PI) || (angle <= limAng1 && angle >= -M_PI))
                {
                  _camera.set_pixel(j, isv->get_color());
                  _camera.set_dist(j, dist);
                }
              }
            }
          }
        }
      }
    }
