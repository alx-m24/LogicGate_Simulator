#version 430 core

#define MAXNODE_NUM 100
#define MAXWIRE_NUM 100
#define MAXGATE_NUM 100

out vec4 FragColor;
uniform float spacing;
uniform float time;
uniform int thickness;
uniform vec2 resolution;
uniform vec2 viewCenter;

struct Node {
    vec2 position;
    vec3 activeColor;
    bool state;
};

uniform Node nodes[MAXNODE_NUM];
uniform int nodeNum;

struct Wire {
    vec2 p1;
    vec2 p2;
    vec3 activeColor;
    bool state;
};

uniform Wire wires[MAXWIRE_NUM];
uniform int wireNum;

struct Gate {
    vec3 color;
    vec2 position;
    vec2 halfSize;
    float radius;
    float outlineThickness;
    vec3 outlineColor;
};

uniform Gate gates[MAXGATE_NUM];
uniform int gateNum;

float map(float value, float min1, float max1, float min2, float max2) {return min2 + (value - min1) * (max2 - min2) / (max1 - min1);}
float roundedBoxSDF(vec2 p, vec2 b, float r) {
    return length(max(abs(p) - b + vec2(r), 0.0)) - r;
}
bool isOnRoundedRect(vec2 p, vec2 b, float r) {
    return roundedBoxSDF(p, b, r) <= 0.0;
}

void main() {
    vec2 position = gl_FragCoord.xy;
    vec2 uv = position / resolution;

    float distFromCenter = length(2.0 * uv - 1.0);

    // Base background color
    FragColor = vec4(vec3(64.0, 61.0, 57.0) / 255.0, 1.0);
    FragColor = vec4(mix(FragColor, FragColor / 2.0, distFromCenter).xyz, 1.0);
    
    // Grid
    float epsilon = 0.5 + thickness;
    vec2 modResult = mod(position + vec2(viewCenter.x, resolution.y - viewCenter.y) + spacing / 2.0, spacing);
    if (modResult.x < epsilon && modResult.y < epsilon) {
        FragColor = vec4(1.0); // White grid
    }

    // Node glow
    for (int i = 0; i < min(nodeNum, MAXNODE_NUM); ++i) {
        Node node = nodes[i];
        if (node.state) {
            float nodeRadius = spacing / 2.0;
            vec2 flippedNodePos = vec2(node.position.x, resolution.y - node.position.y);
            float distFromNode = max(distance(position, flippedNodePos), 0.001); // Avoid division by zero
            float normalizedDist = distFromNode / nodeRadius;

            float pulse = sin(time * 2.0 + i * 10) / 7.0 + (1.0 - 1.0 / 7.0);
            float intensity = smoothstep(0.0, 1.0, pulse * 0.9 / (normalizedDist * normalizedDist)); // Smoothed glow

            vec3 glowColor = node.activeColor / 255.0 * intensity;
            FragColor.rgb = min(FragColor.rgb + glowColor, vec3(1.0)); // Prevent overexposure
        }
    }
    // Wire
    for (int i = 0; i < min(wireNum, MAXWIRE_NUM); ++i) {
        Wire wire = wires[i];
        vec2 p1 = vec2(wire.p1.x, resolution.y - wire.p1.y);
        vec2 p2 = vec2(wire.p2.x, resolution.y - wire.p2.y);

        // Get direction and length of the line segment
        vec2 dir = p2 - p1;
        float lenSq = dot(dir, dir);
        
        // Projection factor (t)
        float t = dot(position - p1, dir) / lenSq;
        t = clamp(t, 0.0, 1.0);
        
        // Closest point on the line segment
        vec2 closest = p1 + t * dir;
        
        // Distance from fragment to closest point
        float distFromLine = max(length(position - closest), 0.001);
        if (wire.state) {
            float pulse = sin((length(closest - p1) - time * 100.0) * 0.05);
            pulse = map(pulse, -1.0, 1.0, 1.5, 3.0);

            float intensity = smoothstep(0.0, 1.0, pulse / (distFromLine * distFromLine)); // Smoothed glow

            vec3 glowColor = wire.activeColor / 255.0 * intensity;
            FragColor.rgb = min(FragColor.rgb + glowColor, vec3(1.0)); // Prevent overexposure
        }
        else {
            if (distFromLine < 1.5) FragColor = vec4(vec3(147.0, 145.0, 150.0) / 255.0, 1.0);
        }
    }
    // Gate
    for (int i = 0; i < min(gateNum, MAXGATE_NUM); ++i) {
        Gate gate = gates[i];
    
        vec2 flippedNodePos = vec2(gate.position.x, resolution.y - gate.position.y);

        // Convert UV to rectangle space (centered at origin)
        vec2 localPos = position - flippedNodePos;

         // Compute SDF for rounded rectangle
        float sdf = roundedBoxSDF(localPos, gate.halfSize, gate.radius);
        
        // Smooth edges for anti-aliasing
        float alpha = smoothstep(0.5, -0.5, sdf);
    
         // Determine if we're inside the outline thickness
        float insideOutline = sdf - gate.outlineThickness;

        // If inside the outline range, use the outline color
        if (sdf < 0.0) {
            FragColor = vec4(gate.color.rgb, 1.0);
        } else if (insideOutline < 0.0) {
            FragColor = vec4(gate.outlineColor, 1.0); // Use outline color
        } else {
            // Smooth edge for fill color
            float alpha = smoothstep(0.5, -0.5, sdf);
            if (alpha > 0.0)
                FragColor = vec4(gate.color.rgb, alpha);
        }
    }
}
