ALTER TABLE tasks ADD COLUMN dedupe_key TEXT;
CREATE UNIQUE INDEX IF NOT EXISTS idx_tasks_active_dedupe
ON tasks(dedupe_key)
WHERE dedupe_key IS NOT NULL AND status IN ('pending','processing');
CREATE INDEX IF NOT EXISTS idx_tasks_locked_at ON tasks(status, locked_at);
