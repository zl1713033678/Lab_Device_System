$pythonExe = "python"
if (Test-Path "$PSScriptRoot\.venv\Scripts\python.exe") {
    $pythonExe = "$PSScriptRoot\.venv\Scripts\python.exe"
} elseif (& py -3.13 -c "import fastapi" 2>$null; $LASTEXITCODE -eq 0) {
    $pythonExe = "py -3.13"
}

Start-Process "http://localhost:8000"
if ($pythonExe -eq "py -3.13") {
    & py -3.13 main.py
} else {
    & $pythonExe main.py
}
