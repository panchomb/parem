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
    avg_exec_time_1_thread = df[df['num_threads'] == 1].groupby('input_length_exp')['exec_time'].mean().reset_index()
    
    # Plot execution times
    plt.figure(figsize=(12, 8))
    for input_length in unique_input_lengths:
        subset = df[df['input_length_exp'] == input_length]
        avg_exec_time = subset.groupby('num_threads')['exec_time'].mean().reset_index()
        
        plt.plot(avg_exec_time['num_threads'], avg_exec_time['exec_time'], marker='o', label=f'Input Length: 2^{input_length}')
    
    plt.xlabel('Number of Threads')
    plt.ylabel('Average Execution Time (seconds)')
    plt.title('Average Execution Time vs Number of Threads')
    plt.grid(True)
    plt.legend()
    plt.savefig('execution_time_plot.png')
    plt.close()

    # Plot speedup times
    plt.figure(figsize=(12, 8))
    for input_length in unique_input_lengths:
        subset = df[df['input_length_exp'] == input_length]
        avg_exec_time = subset.groupby('num_threads')['exec_time'].mean().reset_index()
        
        # Calculate speedup
        baseline_time = avg_exec_time_1_thread[avg_exec_time_1_thread['input_length_exp'] == input_length]['exec_time'].values[0]
        avg_exec_time['speedup'] = baseline_time / avg_exec_time['exec_time']
        
        plt.plot(avg_exec_time['num_threads'], avg_exec_time['speedup'], marker='o', label=f'Input Length: 2^{input_length}')
    
    plt.xlabel('Number of Threads')
    plt.ylabel('Speedup')
    plt.title('Speedup vs Number of Threads')
    plt.grid(True)
    plt.legend()
    plt.savefig('speedup_plot.png')
    plt.close()

# Example usage
data = process_data('output.txt')  # Replace 'output.txt' with the actual file containing the standard output
generate_plots(data)
