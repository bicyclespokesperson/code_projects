use anyhow::Result;
use std::path::Path;
use tokio::fs::OpenOptions;
use tokio::io::AsyncWriteExt;
use uuid::Uuid;

pub async fn append_to_transcript(room_id: Uuid, content: String) -> Result<()> {
    let dir_path = "transcripts";
    if !Path::new(dir_path).exists() {
        tokio::fs::create_dir_all(dir_path).await?;
    }

    let file_path = format!("{}/room_{}.md", dir_path, room_id);
    
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
