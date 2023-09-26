import numpy as np
import matplotlib.pyplot as plt

def generate_speedometer_points(radius, num_points):
    center_x, center_y = 240, 240
    start_angle_deg = -225
    end_angle_deg = 45

    angles_deg = np.linspace(start_angle_deg, end_angle_deg, num_points)
    angles_rad = np.deg2rad(angles_deg)

    x_points = center_x + radius * np.cos(angles_rad)
    y_points = center_y + radius * np.sin(angles_rad)

    # Round the coordinates to the nearest integer
    x_points = np.round(x_points).astype(int)
    y_points = np.round(y_points).astype(int)

    # Combine x and y coordinates into a list of (x, y) tuples
    points = list(zip(x_points, y_points))

    return points

def draw_points(points):
    # for point in points:
    #     x, y = point
    #     print(f"x={x}, y={y}")
    formatted_points = ", ".join([f"{{{x}, {y}}}" for x, y in points])
    print(formatted_points)
    
    x_points, y_points = zip(*points)
    plt.scatter(x_points, y_points, color='blue', marker='o')
    plt.xlabel('X')
    plt.ylabel('Y')
    plt.title('Speedometer Points')
    plt.grid(True)
    plt.axis('equal')
    plt.show()

if __name__ == "__main__":
    radius = 200  # Adjust the radius as needed
    num_points = 70
    speedometer_points = generate_speedometer_points(radius, num_points)
    # print(speedometer_points)
    draw_points(speedometer_points)

# Usage example:
# radius = 200  # Adjust the radius as needed
# num_points = 70
# speedometer_points = generate_speedometer_points(radius, num_points)
# print(speedometer_points)