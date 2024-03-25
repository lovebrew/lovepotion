use std::env;
use std::fs;
use std::process::Command;
use std::time::{SystemTime, UNIX_EPOCH};


fn main() {
    let args: Vec<String> = env::args().collect();

    let ip = match args.get(1) {
        Some(ip) => ip,
        None => {
            eprintln!("IP address of the device is required");
            return;
        }
    };

    fs::create_dir_all("logs").expect("Failed to create logs directory");

    let os = std::env::consts::OS;
    let timestamp = SystemTime::now()
        .duration_since(UNIX_EPOCH)
        .expect("Failed to get current timestamp")
        .as_secs();

    let filename = format!("logs/{}.log", timestamp);

    match os {
        "windows" => {
            Command::new("telnet")
                .args(&[ip, "8000", "-f", &filename])
                .spawn()
                .expect("Failed to start telnet: command not found.");
        }
        "linux" | "macos" => {
            Command::new("netcat")
                .args(&[ip, "8000", ">", &filename, "2>&1"])
                .spawn()
                .expect("Failed to start netcat: command not found.");
        }
        _ => {
            eprintln!("Unsupported operating system: {}", os);
            return;
        }
    }
}
