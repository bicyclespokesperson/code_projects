#!/usr/bin/env python3
"""
Generate a system architecture diagram for the Disc Golf Flight Simulation
"""

import matplotlib
matplotlib.use('Agg')  # Use non-interactive backend
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
from matplotlib.patches import FancyBboxPatch, FancyArrowPatch
import matplotlib.lines as mlines

# Set up the figure
fig, ax = plt.subplots(1, 1, figsize=(16, 12))
ax.set_xlim(0, 16)
ax.set_ylim(0, 12)
ax.axis('off')

# Title
ax.text(8, 11.5, 'Disc Golf Flight Simulation - System Architecture',
        ha='center', va='top', fontsize=20, fontweight='bold')

# Color scheme
color_module = '#E3F2FD'  # Light blue
color_data = '#FFF9C4'    # Light yellow
color_coord = '#E1F5FE'   # Cyan
color_external = '#E8F5E9' # Light green
color_algorithm = '#FCE4EC' # Light pink

def draw_box(x, y, w, h, text, color, fontsize=10, fontweight='normal'):
    """Draw a rounded rectangle box with text"""
    box = FancyBboxPatch((x, y), w, h,
                         boxstyle="round,pad=0.1",
                         edgecolor='black',
                         facecolor=color,
                         linewidth=1.5)
    ax.add_patch(box)
    ax.text(x + w/2, y + h/2, text,
            ha='center', va='center', fontsize=fontsize, fontweight=fontweight,
            multialignment='center')
    return (x + w/2, y + h/2)

def draw_arrow(x1, y1, x2, y2, label='', color='black', linewidth=2):
    """Draw an arrow between two points"""
    arrow = FancyArrowPatch((x1, y1), (x2, y2),
                          arrowstyle='->', mutation_scale=20,
                          linewidth=linewidth, color=color,
                          connectionstyle="arc3,rad=0")
    ax.add_patch(arrow)
    if label:
        mid_x, mid_y = (x1 + x2) / 2, (y1 + y2) / 2
        ax.text(mid_x, mid_y + 0.15, label,
                ha='center', va='bottom', fontsize=8,
                bbox=dict(boxstyle='round,pad=0.3', facecolor='white', edgecolor='gray', alpha=0.8))

# External I/O (top)
json_pos = draw_box(0.5, 10, 2, 0.8, 'discs.json\n(Disc Properties)', color_external, fontsize=9)
output_pos = draw_box(13.5, 10, 2, 0.8, 'flight_plot.bmp\n(Visualization)', color_external, fontsize=9)

# Main module (top center)
main_box = draw_box(6, 9.2, 4, 1.2, 'main.rs\nOrchestrates Simulation', color_module, fontsize=11, fontweight='bold')

# Launch and sim controls (below main)
launch_pos = draw_box(4.5, 7.5, 2.5, 1.2, 'Launch Config\n• speed\n• spin\n• angles\n• height', color_data, fontsize=8)
sim_pos = draw_box(9, 7.5, 2.5, 1.2, 'Sim Controls\n• dt (time step)\n• max_time', color_data, fontsize=8)

# Disc reader module (left side)
disc_reader_title = draw_box(0.5, 7, 3, 0.6, 'disc_reader.rs', '#BBDEFB', fontsize=10, fontweight='bold')
disc_reader_pos = draw_box(0.5, 6, 3, 0.8, 'discs_from_filename()\nParse JSON disc data', color_module, fontsize=9)
disc_js_pos = draw_box(0.5, 4.8, 3, 0.9, 'DiscJS\nSerde Deserializer\n→ Disc struct', color_data, fontsize=8)

# Path simulator module (center/bottom)
sim_title = draw_box(4.5, 6.3, 7, 0.6, 'path_simulator.rs - Core Physics Engine', '#FFB74D', fontsize=10, fontweight='bold')
simulate_pos = draw_box(5.5, 5.2, 5, 0.8, 'simulate()\nIterative Physics Simulation', color_algorithm, fontsize=10, fontweight='bold')

# Data structures (below simulator)
disc_struct_pos = draw_box(4.5, 3.8, 2.5, 1.1, 'Disc\n• AeroProps\n• Mass\n• Diameter\n• Moments of Inertia', color_data, fontsize=8)
sim_step_pos = draw_box(9, 3.8, 2.5, 1.1, 'SimStep\nPer-frame state\n(all coords)', color_data, fontsize=8)

# Coordinate systems (bottom center)
coord_title = draw_box(4.5, 2.9, 7, 0.4, 'Coordinate Systems & Transforms', '#B2DFDB', fontsize=9, fontweight='bold')
ground_pos = draw_box(4.5, 1.8, 1.5, 0.8, 'Ground\nCoords', color_coord, fontsize=8)
disc_pos = draw_box(6.2, 1.8, 1.5, 0.8, 'Disc\nCoords', color_coord, fontsize=8)
sideslip_pos = draw_box(7.9, 1.8, 1.5, 0.8, 'Side-Slip\nCoords', color_coord, fontsize=8)
wind_pos = draw_box(9.6, 1.8, 1.5, 0.8, 'Wind\nCoords', color_coord, fontsize=8)

# Physics calculations
physics_pos = draw_box(4.5, 0.5, 7, 0.8, 'Physics Calculations: Drag, Lift, Pitching Moment, Gravity', color_algorithm, fontsize=9)

# Transforms
transforms_pos = draw_box(12.5, 2, 3, 1.3, 'Coordinate\nTransforms\ngd, dg, ds, sd\nsw, ws', color_algorithm, fontsize=8)

# Plotter module (right side)
plotter_title = draw_box(13, 7, 2.5, 0.6, 'plotter.rs', '#BBDEFB', fontsize=10, fontweight='bold')
plot_pos = draw_box(13, 6, 2.5, 0.8, 'plot()\nGenerate Visualization', color_module, fontsize=9)
plotters_lib_pos = draw_box(13, 4.8, 2.5, 0.9, 'Plotters Library\nChartBuilder\nLineSeries', color_external, fontsize=8)

# Draw arrows - Data flow
draw_arrow(json_pos[0], json_pos[1] - 0.4, disc_reader_pos[0], disc_reader_pos[1] + 0.4, 'read', 'blue', 2)
draw_arrow(disc_reader_pos[0], disc_reader_pos[1] - 0.4, disc_js_pos[0], disc_js_pos[1] + 0.45, 'deserialize', 'blue', 1.5)
draw_arrow(disc_js_pos[0] + 1.5, disc_js_pos[1], main_box[0] - 2, main_box[1] - 0.3, 'Vec<Disc>', 'blue', 2.5)

# Main to simulator
draw_arrow(main_box[0], main_box[1] - 0.6, simulate_pos[0], simulate_pos[1] + 0.4, 'disc, launch,\ncontrols', 'red', 3)
draw_arrow(launch_pos[0] + 1.25, launch_pos[1], simulate_pos[0] - 1.5, simulate_pos[1], '', 'gray', 1.5)
draw_arrow(sim_pos[0] - 1.25, sim_pos[1], simulate_pos[0] + 1.5, simulate_pos[1], '', 'gray', 1.5)

# Simulator to data structures
draw_arrow(simulate_pos[0] - 1, simulate_pos[1] - 0.4, disc_struct_pos[0], disc_struct_pos[1] + 0.55, 'uses', 'gray', 1.5)
draw_arrow(simulate_pos[0] + 1, simulate_pos[1] - 0.4, sim_step_pos[0], sim_step_pos[1] + 0.55, 'creates', 'gray', 1.5)

# Simulator to coordinates
draw_arrow(simulate_pos[0] - 1, simulate_pos[1] - 0.4, ground_pos[0], ground_pos[1] + 0.4, '', 'purple', 1.5)
draw_arrow(simulate_pos[0], simulate_pos[1] - 0.4, disc_pos[0], disc_pos[1] + 0.4, '', 'purple', 1.5)
draw_arrow(simulate_pos[0], simulate_pos[1] - 0.4, sideslip_pos[0], sideslip_pos[1] + 0.4, '', 'purple', 1.5)
draw_arrow(simulate_pos[0] + 1, simulate_pos[1] - 0.4, wind_pos[0], wind_pos[1] + 0.4, '', 'purple', 1.5)

# Coordinates to transforms
draw_arrow(ground_pos[0] + 0.75, ground_pos[1], transforms_pos[0] - 1.5, transforms_pos[1] + 0.3, '', 'purple', 1.5)
draw_arrow(wind_pos[0] + 0.75, wind_pos[1], transforms_pos[0] - 1.5, transforms_pos[1] - 0.3, '', 'purple', 1.5)

# Transforms to physics
draw_arrow(transforms_pos[0] - 1.5, transforms_pos[1] - 0.65, physics_pos[0] + 3.5, physics_pos[1] + 0.4, 'coordinate\nconversions', 'purple', 1.5)

# Physics feedback to simulator
draw_arrow(physics_pos[0] - 3, physics_pos[1] + 0.4, simulate_pos[0] - 2, simulate_pos[1] - 0.4, 'updates\nstate', 'purple', 1.5)

# Simulator to plotter
draw_arrow(simulate_pos[0] + 2.5, simulate_pos[1] + 0.3, plot_pos[0] - 1.25, plot_pos[1], 'Vec<Ground>', 'green', 2.5)
draw_arrow(main_box[0] + 2, main_box[1] - 0.3, plot_pos[0], plot_pos[1] + 0.4, 'call plot()', 'green', 2)

# Plotter to output
draw_arrow(plot_pos[0], plot_pos[1] - 0.4, plotters_lib_pos[0], plotters_lib_pos[1] + 0.45, 'uses', 'green', 1.5)
draw_arrow(plotters_lib_pos[0], plotters_lib_pos[1] + 0.45, output_pos[0], output_pos[1] - 0.4, 'renders', 'green', 2)

# Add legend
legend_x, legend_y = 0.5, 0.3
ax.text(legend_x, legend_y + 1.5, 'Legend:', fontsize=11, fontweight='bold')

legend_items = [
    (color_module, 'Module/Function'),
    (color_data, 'Data Structure'),
    (color_coord, 'Coordinate System'),
    (color_algorithm, 'Algorithm'),
    (color_external, 'External/Library'),
]

for i, (color, label) in enumerate(legend_items):
    y = legend_y + 1.1 - i * 0.2
    small_box = FancyBboxPatch((legend_x, y - 0.08), 0.3, 0.15,
                               boxstyle="round,pad=0.02",
                               edgecolor='black',
                               facecolor=color,
                               linewidth=1)
    ax.add_patch(small_box)
    ax.text(legend_x + 0.4, y, label, fontsize=8, va='center')

# Add notes
notes_text = """Key Flow:
1. Load disc data from JSON
2. Configure launch parameters
3. Run physics simulation
   (coordinate transforms)
4. Generate visualization"""

ax.text(13.5, 1.2, notes_text, fontsize=8,
        bbox=dict(boxstyle='round,pad=0.5', facecolor='#FFF3E0', edgecolor='black', linewidth=1),
        verticalalignment='top', family='monospace')

plt.tight_layout()
plt.savefig('/home/user/code_projects/disc_golf_flight_simulation/system_diagram.pdf',
            format='pdf', dpi=300, bbox_inches='tight')
print("System diagram PDF generated successfully!")
