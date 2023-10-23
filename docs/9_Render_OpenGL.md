#### Client's function, see [This repository](https://github.com/RGAA-Software/sailfish_client_pc)

#### If you are unfamiliar with OpenGL now, please see [Here(LearnOpenGL)](https://learnopengl.com/)

After decode, we'll get I420 frame buffer, a YUV buffer. The code is here : src/ui/OpenGLVideoWidget.cpp  
1. We store the I420 data in separate channels, so the initialize method like this:
```c++
void OpenGLVideoWidget::InitI420Texture() {
    auto create_luminance_texture = [this](GLuint& tex_id, int width, int height, bool is_uv) {
        glGenTextures(1, &tex_id);
        glBindTexture(GL_TEXTURE_2D, tex_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // !!!Important GL_RED for one channel!!!
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, is_uv ? width / 2 : width, is_uv ? height / 2 : height, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
        glBindTexture(GL_TEXTURE_2D, 0);
    };
    // call 3 times to init 3 buffers
    create_luminance_texture(y_texture_id, tex_width, tex_height, false);
    create_luminance_texture(u_texture_id, tex_width, tex_height, true);
    create_luminance_texture(v_texture_id, tex_width, tex_height, true);
}
```

2. Flush every frame to GPU
```c++
...
    else if (raw_image_format == RawImageFormat::kI420) {

        if (y_buffer && u_buffer && v_buffer && need_create_texture) {
            need_create_texture = false;
            InitI420Texture();
        }

        if (y_buffer) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, y_texture_id);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, tex_width, tex_height, GL_RED, GL_UNSIGNED_BYTE, y_buffer);
        }
        if (u_buffer) {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, u_texture_id);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, tex_width/2, tex_height/2, GL_RED, GL_UNSIGNED_BYTE, u_buffer);
        }
        if (v_buffer) {
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, v_texture_id);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, tex_width / 2, tex_height / 2, GL_RED, GL_UNSIGNED_BYTE, v_buffer);
        }
    }
...
```

3. The fragment shader, two formulae are both ok.
```glsl
 #version 330

    in vec3 outColor;
    in vec2 outTex;

    uniform sampler2D imageY;
    uniform sampler2D imageU;
    uniform sampler2D imageV;

    out vec4 FragColor;

    void main()
    {   
        float y, u, v, r, g, b;
        y = texture(imageY, outTex).r;
        u = texture(imageU, outTex).r;
        v = texture(imageV, outTex).r;        

// formula  1.  
//        y = 1.164 * (y - 16.0 / 255.0);
//        u = u - 128.0 / 255.0;
//        v = v - 128.0 / 255.0;

//        r = y + 1.596 * v;
//        g = y - 0.391 * u - 0.813 * v;
//        b = y + 2.018 * u;

// formula  2.  
        r = y + 1.402 * (v - 0.5);
        g = y - 0.344136 * (u - 0.5) - 0.714136 * (v - 0.5);
        b = y + 1.772 * (u - 0.5);

        //FragColor = vec4(r, g, b, 1.0); 
        FragColor = vec4(r, g, b, 1.0);
        //FragColor = vec4(1.0, outColor.r, outTex.g, 1.0);
        //FragColor = vec4(r, r, r, 1.0);
    }
```
