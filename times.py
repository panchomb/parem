import matplotlib.pyplot as plt
import pandas as pd

# Sample data processing function
def process_data(file_path):
    data = []
    with open(file_path, 'r') as file:
        for line in file:
            parts = line.strip().split()
            if len(parts) == 3:
                num_threads = int(parts[0])
                input_length_exp = int(parts[1])
                exec_time = float(parts[2])
                data.append((num_threads, input_length_exp, exec_time))
    return data

# Generate plots
def generate_plots(data):
    df = pd.DataFrame(data, columns=['num_threads', 'input_length_exp', 'exec_time'])
    
    unique_input_lengths = df['input_length_exp'].unique()
    
    for input_length in unique_input_lengths:
        subset = df[df['input_length_exp'] == input_length]
        avg_exec_time = subset.groupby('num_threads')['exec_time'].mean().reset_index()
        
        plt.figure(figsize=(10, 6))
        plt.plot(avg_exec_time['num_threads'], avg_exec_time['exec_time'], marker='o')
        plt.title(f'Average Execution Time vs Number of Threads (Input Length: 2^{input_length})')
        plt.xlabel('Number of Threads')
        plt.ylabel('Average Execution Time (seconds)')
        plt.grid(True)
        plt.savefig(f'plot_input_length_2^{input_length}.png')
        plt.close()

# Example usage
data = process_data('times.txt')  # Replace 'output.txt' with the actual file containing the standard output
generate_plots(data)
