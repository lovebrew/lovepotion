use std::time::{SystemTime, UNIX_EPOCH};
use std::io::{Read, Write};
use std::net::TcpStream;
use std::fs::File;
use std::fs;


fn main() {
    let args: Vec<String> = std::env::args().collect();

    let ip = match args.get(1) {
        Some(ip) => ip,
        None => {
            eprintln!("IP address of the device is required");
            return;
        }
    };

    let verbose = match args.get(2) {
        Some(verbose) => verbose.parse().unwrap_or(false),
        None => false,
    };

    fs::create_dir_all("logs").expect("Failed to create logs directory");

    let timestamp = SystemTime::now()
        .duration_since(UNIX_EPOCH)
        .expect("Failed to get current timestamp")
        .as_secs();

    let filename = format!("logs/{}.log", timestamp);

    let mut stream = TcpStream::connect(format!("{}:8000", ip))
        .expect("Failed to connect to the IP address");

    let mut buffer = [0; 1024];
    let mut file = File::create(&filename).expect("Failed to create log file");

    loop {
        let bytes_read = stream.read(&mut buffer).expect("Failed to read data from stream");

        if bytes_read == 0 {
            break;
        }

        let data = &buffer[..bytes_read];

        if verbose {
            print!("{}", String::from_utf8_lossy(data));
        }

        file.write_all(data).expect("Failed to write data to file");
    }
}
