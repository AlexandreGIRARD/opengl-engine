#version 450 core

layout (local_size_x = 1024, local_size_y = 1, local_size_z = 1) in;


struct boid_t
{
    vec3 pos;
    vec3 dir;
};

layout (std430, binding = 0) buffer Positions
{
    vec4 pos[];
};

layout (std430, binding = 1) buffer Directions
{
    vec4 dir[];
};

layout (std430, binding = 2) buffer Matrices
{
    mat4 mat[];
};

uniform uint max_boids;
uniform float bound;
uniform float speed_max;
uniform float angle_max;
uniform float separation, alignment, cohesion;
uniform float separation_factor;
uniform float align_factor;
uniform float cohesion_factor;

uniform vec3 follow_obj;
uniform float follow_factor;

uniform vec3 predator_obj;
uniform float predator_factor;
uniform float predator_dist;


mat4 translate(vec3 v)
{
    mat4 m = mat4(1.0);
    m[3].xyz = v;
    return m;
}

mat4 rotate(float angle, vec3 axis)
{
    const float a = angle;
    const float c = cos(a);
    const float s = sin(a);

    vec3 u   = normalize(axis);
    vec3 tmp = (1 - c) * u;

    mat4 r = mat4(1);

    // First column
    r[0][0] = c + u.x * tmp.x;
    r[0][1] = u.y * tmp.x + u.z * s;
    r[0][2] = u.z * tmp.x - u.y * s;

    // Second column
    r[1][0] = u.x * tmp.y - u.z * s;
    r[1][1] = c + u.y * tmp.y;
    r[1][2] = u.z * tmp.y + u.x * s;

    // Third column
    r[2][0] = u.x * tmp.z + u.y * s;
    r[2][1] = u.y * tmp.z - u.x * s;
    r[2][2] = c + u.z * tmp.z;

    return r;
}

float angle(vec3 x, vec3 y)
{
    return acos(clamp(dot(x, y), -1, 1));
}

mat4 get_model_matrix(boid_t boid)
{
    // Generate translation matrix
    mat4 trans_mat = translate(boid.pos);
    // Generate rotatation matrix from ogirin firection (1,0,0) and dir vector
    vec3 normal_dir = normalize(boid.dir);
    float angle = angle(vec3(0,0,1), normal_dir);
    vec3 axis = cross(vec3(0,0,1), normal_dir);
    mat4 rotat_mat = rotate(angle, axis);
    return trans_mat * rotat_mat;
}

boid_t get_boid(uint gid)
{
    boid_t b;
    b.pos = vec3(pos[gid]);
    b.dir = vec3(dir[gid]);
    return b;
}

void update_boid(uint gid, boid_t b)
{
    pos[gid] = vec4(b.pos, 1.0);
    dir[gid] = vec4(b.dir, 1.0);
}

// Center boids with neighbors
vec3 center_boid(boid_t boid, int size, vec3 center)
{
    if (size == 0)
        return vec3(0);
    center /= size;
    return (center - boid.pos) * cohesion_factor;
}

// Align boids with neighbors
vec3 align_boid(boid_t boid, int size, vec3 align)
{
    if (size == 0)
        return vec3(0);
    align /= size;
    return align * align_factor;
}

// Separe current boid from others
vec3 separe_boid(boid_t boid, int size, vec3 move)
{
    // if (size != 0)
    if (size == 0)
        return vec3(0);
    move /= size;
    return move * separation_factor;
    // boid.dir += (boid.pos - move) * separation_factor;
}

void clamp_speed(inout boid_t boid)
{
    const float speed = length(boid.dir);
    if (speed >= speed_max)
        boid.dir = (boid.dir / speed) * speed_max;
}

void check_bound(inout boid_t boid)
{
    vec3 center_sphere = vec3(0);
    if (distance(boid.pos, center_sphere) < bound)
        return;
    vec3 normal = normalize(center_sphere - boid.pos);
    vec3 direction =  boid.dir - dot(boid.dir, normal) * normal;
    vec3 friction =  dot(boid.dir, normal) * normal;
    // _swarm[i].dir = normalize(0.75f * direction - 0.0f * friction);
    boid.dir = reflect(boid.dir, normal);
    while(distance(boid.pos, center_sphere) >= bound)
        boid.pos += 0.05f*normal;
}

bool in_field_of_view(boid_t b1, boid_t b2)
{
    vec3 b1_view = normalize(b1.dir);
    vec3 b1_to_b2 = normalize(b2.pos - b1.pos);
    return angle(b1_to_b2, b1_view) <= angle_max;
}

void main()
{
    uint gid = gl_GlobalInvocationID.x;
    if (gid < max_boids)
    {
        // Init current boid
        boid_t b1 = get_boid(gid);

        // Follow
        b1.dir += normalize(follow_obj - b1.pos) * follow_factor;
        // clamp_speed(b1);
        update_boid(gid, b1);
        barrier();

        // Update current boid
        uint id = gid;
        vec3 align  = vec3(0);
        vec3 center = vec3(0);
        vec3 separe = vec3(0);
        int sep_size, align_size, center_size = 0;
        bool in_field = true;
        do
        {
            // increment id
            id = (id + 1) % max_boids;
            boid_t b2 = get_boid(id);

            if (!in_field_of_view(b1, b2))
                in_field = false;

            float dist = distance(b2.pos, b1.pos);
            if (dist <= separation)
            {
                separe += (b2.pos - b1.pos);
                sep_size++;
            }
            else if (dist <= alignment && in_field)
            {
                align += b2.dir;
                align_size++;
            }
            else if (dist <= cohesion && in_field)
            {
                center += b2.pos;
                center_size++;
            }
        } while (id != gid);

        barrier();
        vec3 v1 = center_boid(b1, center_size, center);
        vec3 v2 = align_boid(b1, align_size, align);
        vec3 v3 = separe_boid(b1, sep_size, separe);

        b1.dir +=  v1 + v2 + v3;
        clamp_speed(b1);

        if (distance(b1.pos, predator_obj) <= predator_dist)
            b1.dir -= predator_factor * normalize(predator_obj - b1.pos);
        b1.pos += b1.dir;
        check_bound(b1);

        update_boid(gid, b1);

        // Change model matrix for draw
        mat[gid] = get_model_matrix(b1);
    }
}
