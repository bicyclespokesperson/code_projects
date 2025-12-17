use anyhow::Result;
use std::path::Path;
use tokio::fs::OpenOptions;
use tokio::io::AsyncWriteExt;
use uuid::Uuid;

pub async fn append_to_transcript(room_id: Uuid, room_name: &str, content: String) -> Result<()> {
    let dir_path = "transcripts";
    if !Path::new(dir_path).exists() {
        tokio::fs::create_dir_all(dir_path).await?;
    }

    let sanitized_name = sanitize_filename(room_name);
    let file_path = format!("{}/{}_{}.md", dir_path, sanitized_name, room_id);
    
    let mut file = OpenOptions::new()
        .create(true)
        .append(true)
        .open(file_path)
        .await?;

    // Add timestamp
    let timestamp = chrono::Utc::now().format("%Y-%m-%d %H:%M:%S UTC");
    let formatted_content = format!("\n[{}] {}\n", timestamp, content);

    file.write_all(formatted_content.as_bytes()).await?;

    Ok(())
}

fn sanitize_filename(name: &str) -> String {
    name.chars()
        .map(|c| if c.is_alphanumeric() { c } else { '_' })
        .collect::<String>()
        .to_lowercase()
}
