#!/usr/bin/python3

import os
from urllib.parse import parse_qs

# Extract query string from environment
query_string = os.getenv('QUERY_STRING', '')

# Parse the query string to get the 'text' parameter
params = parse_qs(query_string)
text = params.get('text', ['Webserve CGI script'])[0]  # Default to 'Fireworks Simulation' if 'text' is not provided

# HTML and JavaScript for fireworks simulation with animated text converging from random positions and styles
print(f"""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Webserv CGI Script</title>
    <style>
        body, html {{
            margin: 0;
            padding: 0;
            height: 100%;
            overflow: hidden;
            font-family: Monospace;
        }}

        #text-container {{
            position: absolute;
            top: 50%;
            left: 50%;
            transform: translate(-50%, -50%);
            white-space: nowrap;
            z-index: 10;
        }}

        .letter {{
            display: inline-block;
            position: absolute;
            transition: all 3s ease-out;
            color: green;
        }}

        canvas {{
            position: absolute;
            top: 0;
            left: 0;
            width: 100%;
            height: 100%;
            z-index: 0;
        }}
    </style>
</head>
<body>
    <div id="text-container"></div>
    <canvas id="fireworksCanvas"></canvas>

    <script>
        const canvas = document.getElementById('fireworksCanvas');
        const ctx = canvas.getContext('2d');
        let particles = [];

        // Resize the canvas to fit the entire window
        function resizeCanvas() {{
            canvas.width = window.innerWidth;
            canvas.height = window.innerHeight;
        }}
        window.addEventListener('resize', resizeCanvas);
        resizeCanvas();

        class Particle {{
            constructor(x, y, dx, dy, color) {{
                this.x = x;
                this.y = y;
                this.dx = dx;
                this.dy = dy;
                this.alpha = 1;
                this.color = color;
            }}

            draw() {{
                ctx.save();
                ctx.globalAlpha = this.alpha;
                ctx.beginPath();
                ctx.arc(this.x, this.y, 3, 0, Math.PI * 2);
                ctx.fillStyle = this.color;
                ctx.fill();
                ctx.restore();
            }}

            update() {{
                this.x += this.dx;
                this.y += this.dy;
                this.alpha -= 0.01;
                this.draw();
            }}
        }}

        function explode(x, y) {{
            const colors = ['red', 'blue', 'green', 'yellow', 'orange', 'purple'];
            for (let i = 0; i < 100; i++) {{
                const angle = Math.random() * Math.PI * 2;
                const speed = Math.random() * 4 + 1;
                const dx = Math.cos(angle) * speed;
                const dy = Math.sin(angle) * speed;
                const color = colors[Math.floor(Math.random() * colors.length)];
                particles.push(new Particle(x, y, dx, dy, color));
            }}
        }}

        canvas.addEventListener('click', (e) => {{
            const x = e.clientX;
            const y = e.clientY;
            explode(x, y);
        }});

        function animate() {{
            ctx.clearRect(0, 0, canvas.width, canvas.height);
            particles = particles.filter(p => p.alpha > 0);
            particles.forEach(p => p.update());
            requestAnimationFrame(animate);
        }}

        animate();

        // Get the text from the query string
        const queryText = "{text}";  // Using query string

        const textContainer = document.getElementById('text-container');
        const randomFonts = ['Arial', 'Verdana', 'Courier New', 'Georgia', 'Times New Roman', 'Monospace'];
        
        // Only create letters if queryText is not empty
        if (queryText) {{
			for (let i = 0; i < queryText.length; i++) {{
				const letter = document.createElement('span');
				letter.textContent = queryText[i];
				letter.style.fontSize = Math.floor(Math.random() * 30 + 100) + 'px';
				letter.style.fontFamily = randomFonts[Math.floor(Math.random() * randomFonts.length)];
				letter.style.top = Math.random() * window.innerHeight + 'px';
				letter.style.left = Math.random() * window.innerWidth + 'px';
				textContainer.appendChild(letter);

				window.setTimeout(() => {{
					letter.style.top = window.innerHeight / 2 + 'px';
					letter.style.left = window.innerWidth / 2 + 'px';
					letter.style.color = 'rgb(' + Math.random() * 256 + ',' + Math.random() * 256 + ',' + Math.random() * 256 + ')';
				}}, 100);

				setTimeout(() => {{
					letter.style.transition = 'all 3s ease-out';
					letter.style.top = window.innerHeight / 2 + 'px';
					letter.style.left = window.innerWidth / 2 + 'px';
				}}, 200);

				document.body.style.backgroundColor = 'rgb(' + Math.random() * 256 + ',' + Math.random() * 256 + ',' + Math.random() * 256 + ')';

				particles = [];
				explode(window.innerWidth / 2, window.innerHeight / 2);
			}}
		}}
    </script>
</body>
</html>
""")
