
using CSyntax
using Dates
using GLFW
using ModernGL

const DEFAULT_R = 0.8
const FRAGMENT_SHADER_BW = "fragment_shader_bw.glsl"
const FRAGMENT_SHADER_COLORFUL = "fragment_shader_colorful.glsl"
const VERTEX_SHADER = "vertex_shader.glsl"

function glsetup()
    # set up OpenGL context version
    # it seems OpenGL 4.1 is the highest version supported by MacOS.
    @static if Sys.isapple()
        VERSION_MAJOR = 4
        VERSION_MINOR = 1
    end

    @static if Sys.isapple()
        GLFW.WindowHint(GLFW.CONTEXT_VERSION_MAJOR, VERSION_MAJOR)
        GLFW.WindowHint(GLFW.CONTEXT_VERSION_MINOR, VERSION_MINOR)
        GLFW.WindowHint(GLFW.OPENGL_PROFILE, GLFW.OPENGL_CORE_PROFILE)
        GLFW.WindowHint(GLFW.OPENGL_FORWARD_COMPAT, GL_TRUE)
    else
        GLFW.DefaultWindowHints()
    end
end

function get_radius(args)
    if length(args) == 0
        return DEFAULT_R
    end
    try
        r = parse(Float64, args[1])
        return if r > 0 r else DEFAULT_R end
    catch
        return DEFAULT_R
    end
end

function get_fragment_shader(args)
    if length(args) >= 2 && args[2] == "bw"
        return FRAGMENT_SHADER_BW
    end
    return FRAGMENT_SHADER_COLORFUL
end

function create_shader_program(fs_source, vs_source)
    vs = glCreateShader(GL_VERTEX_SHADER)
    glShaderSource(vs, 1, Ptr{GLchar}[pointer(vs_source)], C_NULL)
    glCompileShader(vs)
    fs = glCreateShader(GL_FRAGMENT_SHADER)
    glShaderSource(fs, 1, Ptr{GLchar}[pointer(fs_source)], C_NULL)
    glCompileShader(fs)
    program = glCreateProgram()
    glAttachShader(program, vs)
    glAttachShader(program, fs)
    glLinkProgram(program)
    return program
end

mutable struct Controls
    right::Bool
    up::Bool
    left::Bool
    down::Bool
    zoomin::Bool
    zoomout::Bool
    paused::Bool
    backward::Bool
    forward::Bool
    max_iter_increase::Bool
    max_iter_decrease::Bool
    print::Bool
    quit::Bool
end

Controls() = Controls(false, false, false, false, false, false, false, false, 
    false, false, false, false, false)

function make_key_listener(controls) 
    function key_listener(window, key, a, action, b)
        if action == GLFW.PRESS
            if key == GLFW.KEY_I
                controls.zoomin = true
            elseif key == GLFW.KEY_O
                controls.zoomout = true
            elseif key == GLFW.KEY_D
                controls.right = true
            elseif key == GLFW.KEY_W
                controls.up = true
            elseif key == GLFW.KEY_A
                controls.left = true
            elseif key == GLFW.KEY_S
                controls.down = true
            elseif key == GLFW.KEY_SPACE
                controls.paused = !controls.paused
            elseif key == GLFW.KEY_LEFT
                controls.backward = true
            elseif key == GLFW.KEY_RIGHT
                controls.forward = true
            elseif key == GLFW.KEY_UP
                controls.max_iter_increase = true
            elseif key == GLFW.KEY_DOWN
                controls.max_iter_decrease = true
            elseif key == GLFW.KEY_P
                controls.print = true
            elseif key == GLFW.KEY_Q
                controls.quit = true
            end
        elseif action == GLFW.RELEASE
            if key == GLFW.KEY_I
                controls.zoomin = false
            elseif key == GLFW.KEY_O
                controls.zoomout = false
            elseif key == GLFW.KEY_D
                controls.right = false
            elseif key == GLFW.KEY_W
                controls.up = false
            elseif key == GLFW.KEY_A
                controls.left = false
            elseif key == GLFW.KEY_S
                controls.down = false
            elseif key == GLFW.KEY_LEFT
                controls.backward = false
            elseif key == GLFW.KEY_RIGHT
                controls.forward = false
            elseif key == GLFW.KEY_UP
                controls.max_iter_increase = false
            elseif key == GLFW.KEY_DOWN
                controls.max_iter_decrease = false
            end
        end
    end
end

function main()
    
    glsetup()
    
    window = GLFW.CreateWindow(720, 720, "Julia Set Explorer")
    @assert window != C_NULL "could not open window with GLFW3."
    GLFW.MakeContextCurrent(window)

    glEnable(GL_DEPTH_TEST)
    glDepthFunc(GL_LESS)

    program = create_shader_program(read(get_fragment_shader(ARGS), String),
        read(VERTEX_SHADER, String))

    screen_rect = GLfloat[
        -1.0, -1.0, 0.0,
        1.0, -1.0, 0.0,
        1.0, 1.0, 0.0,
        -1.0, -1.0, 0.0,
        1.0, 1.0, 0.0,
        -1.0, 1.0, 0.0
    ]

    vbo = GLuint(0)
    @c glGenBuffers(1, &vbo)
    glBindBuffer(GL_ARRAY_BUFFER, vbo)
    glBufferData(GL_ARRAY_BUFFER, sizeof(screen_rect), screen_rect, 
        GL_STATIC_DRAW)

    vao = GLuint(0)
    @c glGenVertexArrays(1, &vao)
    glBindVertexArray(vao)
    glBindBuffer(GL_ARRAY_BUFFER, vbo)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, C_NULL)
    glEnableVertexAttribArray(0)

    glClearColor(0.0, 0.0, 0.0, 1.0)

    # angular frequency
    w = 0.1 * 2 * pi 
    r = get_radius(ARGS)

    controls = Controls()
    GLFW.SetKeyCallback(window, make_key_listener(controls))
    t0 = now()
    tm = t0
    c = r * exp(0im)
    center = 0 + 0im
    centerdot = 2.0
    scale = 0.5
    scalebase = 3.0
    max_iter = 200.0
    max_iter_dot = 20.0
    while !GLFW.WindowShouldClose(window)

        if controls.quit
            break
        end

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        
        glUseProgram(program)
        
        tn = now()
        t = (tn - t0).value / 1000.0
        dt = (tn - tm).value / 1000.0
        if !controls.paused && !controls.backward && !controls.forward
            c = c * exp(im * w * dt)
        end
        if controls.forward
            c = c * exp(2 * im * w * dt)
        end
        if controls.backward
            c = c * exp(-2 * im * w * dt)
        end
        if controls.zoomin
            scale += scale * log(scalebase) * dt
        end
        if controls.zoomout
            scale -= scale * log(scalebase) * dt
        end
        if controls.right
            center += centerdot * dt / scale
        end
        if controls.up
            center += im * centerdot * dt / scale
        end
        if controls.left
            center -= centerdot * dt / scale
        end
        if controls.down
            center -= im * centerdot * dt / scale
        end
        if controls.max_iter_increase
            max_iter += max_iter_dot * dt
        end
        if controls.max_iter_decrease
            max_iter -= max_iter_dot * dt
        end
        if controls.print
            println("c = $c")
            println("Showing the section between " * 
                "$(round(center - (1 + 1im)/scale, digits=5)) and " *
                "$(round(center + (1 + 1im)/scale, digits=5))")
            controls.print = false
        end
        tm = tn

        loc = glGetUniformLocation(program, "max_iter")
        glUniform1i(loc, floor(max_iter))
        loc = glGetUniformLocation(program, "c")
        glUniform2f(loc, real(c), imag(c))
        loc = glGetUniformLocation(program, "center")
        glUniform2f(loc, real(center), imag(center))
        loc = glGetUniformLocation(program, "scale")
        glUniform1f(loc, scale)
        
        glBindVertexArray(vao)
        glDrawArrays(GL_TRIANGLES, 0, 6)
        
        GLFW.PollEvents()
        GLFW.SwapBuffers(window)
    end

    GLFW.DestroyWindow(window)

end

main()
