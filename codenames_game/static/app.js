// ===== Codenames Game Client =====

class CodenamesGame {
    constructor() {
        this.ws = null;
        this.playerId = null;
        this.roomId = null;
        this.gameState = null;
        this.playerName = '';
        this.selectedRoom = null;
        this.apiKey = localStorage.getItem('openrouter_api_key') || '';
        this.isAiProcessing = false;
        this.isAutoPlaying = false;

        this.init();
    }

    init() {
        this.bindEvents();
        this.loadApiKey();
        this.fetchRooms();
        this.fetchModels();
    }

    // ===== Event Binding =====
    bindEvents() {
        // Landing screen
        document.getElementById('create-room-btn').addEventListener('click', () => this.createRoom());
        document.getElementById('refresh-rooms-btn').addEventListener('click', () => this.fetchRooms());
        document.getElementById('api-key').addEventListener('change', (e) => this.saveApiKey(e.target.value));

        // Lobby screen
        document.getElementById('leave-lobby-btn').addEventListener('click', () => this.leaveLobby());
        document.querySelectorAll('.join-team-btn').forEach(btn => {
            btn.addEventListener('click', (e) => this.joinTeam(e.target.dataset.team, e.target.dataset.role));
        });
        document.getElementById('add-ai-btn').addEventListener('click', () => this.addAiPlayer());
        document.getElementById('quick-populate-btn').addEventListener('click', () => this.quickPopulate());
        document.getElementById('start-game-btn').addEventListener('click', () => this.startGame());
        document.getElementById('ai-model-filter').addEventListener('keypress', (e) => {
            if (e.key === 'Enter') this.filterModels(e.target.value);
        });

        // Game screen
        document.getElementById('give-clue-btn').addEventListener('click', () => this.giveClue());
        document.getElementById('pass-turn-btn').addEventListener('click', () => this.passTurn());
        document.getElementById('trigger-ai-btn').addEventListener('click', () => this.triggerAiAction());
        document.getElementById('autoplay-btn').addEventListener('click', () => this.startAutoPlay());
        document.getElementById('stop-autoplay-btn').addEventListener('click', () => this.stopAutoPlay());
        document.getElementById('reset-game-btn').addEventListener('click', () => this.resetGame());
        document.getElementById('send-chat-btn').addEventListener('click', () => this.sendChat());
        document.getElementById('chat-input').addEventListener('keypress', (e) => {
            if (e.key === 'Enter') this.sendChat();
        });
        document.getElementById('clue-word').addEventListener('keypress', (e) => {
            if (e.key === 'Enter') this.giveClue();
        });

        // Game over modal
        document.getElementById('play-again-btn').addEventListener('click', () => this.resetGame());
        document.getElementById('back-to-lobby-btn').addEventListener('click', () => this.backToLobby());

        // Enter key for inputs
        document.getElementById('room-name').addEventListener('keypress', (e) => {
            if (e.key === 'Enter') this.createRoom();
        });
        document.getElementById('player-name-create').addEventListener('keypress', (e) => {
            if (e.key === 'Enter') this.createRoom();
        });

        // Toggle groups
        this.setupToggle('ai-team-toggle', 'ai-team');
        this.setupToggle('ai-role-toggle', 'ai-role');
    }

    setupToggle(containerId, inputId) {
        const container = document.getElementById(containerId);
        const input = document.getElementById(inputId);
        if (!container || !input) return;

        container.querySelectorAll('.btn-toggle').forEach(btn => {
            btn.addEventListener('click', () => {
                // Remove active class from siblings
                container.querySelectorAll('.btn-toggle').forEach(b => b.classList.remove('active'));
                // Add active class to clicked
                btn.classList.add('active');
                // Update hidden input
                input.value = btn.dataset.value;
            });
        });
    }

    // ===== API Key Management =====
    loadApiKey() {
        const saved = localStorage.getItem('openrouter_api_key');
        if (saved) {
            document.getElementById('api-key').value = saved;
            this.apiKey = saved;
        }
    }

    saveApiKey(key) {
        this.apiKey = key;
        localStorage.setItem('openrouter_api_key', key);
    }

    // ===== HTTP API Calls =====
    async fetchRooms() {
        try {
            const response = await fetch('/api/rooms');
            const data = await response.json();

            if (data.success) {
                this.renderRoomList(data.data);
            }
        } catch (error) {
            console.error('Failed to fetch rooms:', error);
            this.showToast('Failed to load rooms', 'error');
        }
    }

    async fetchModels() {
        try {
            const response = await fetch('/api/models');
            const data = await response.json();

            if (data.success) {
                this.availableModels = data.data;
                this.renderModelSelect(this.availableModels);
            }
        } catch (error) {
            console.error('Failed to fetch models:', error);
        }
    }

    async createRoom() {
        const roomName = document.getElementById('room-name').value.trim();
        const playerName = document.getElementById('player-name-create').value.trim();

        if (!roomName || !playerName) {
            this.showToast('Please enter room name and your name', 'error');
            return;
        }

        try {
            const response = await fetch('/api/rooms', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ name: roomName, host_name: playerName })
            });

            const data = await response.json();

            if (data.success) {
                this.roomId = data.data.room_id;
                this.playerId = data.data.host_id;
                this.playerName = playerName;
                this.connectWebSocket();
            } else {
                this.showToast(data.error || 'Failed to create room', 'error');
            }
        } catch (error) {
            console.error('Failed to create room:', error);
            this.showToast('Failed to create room', 'error');
        }
    }

    async addAiPlayer() {
        if (!this.roomId) return;

        let name = document.getElementById('ai-name').value.trim();
        const modelSelect = document.getElementById('ai-model');
        
        if (!name && modelSelect.selectedIndex >= 0) {
            name = modelSelect.options[modelSelect.selectedIndex].text;
        }
        
        if (!name) name = 'AI Player';

        const team = document.getElementById('ai-team').value;
        const role = document.getElementById('ai-role').value;
        const model = document.getElementById('ai-model').value;

        try {
            const response = await fetch(`/api/rooms/${this.roomId}/ai-player`, {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({
                    name,
                    team,
                    role,
                    model,
                    api_key: this.apiKey || null
                })
            });

            const data = await response.json();

            if (data.success) {
                this.showToast(`Added AI player: ${name}`, 'success');
                // Request updated state
                this.send({ type: 'request_state' });
            } else {
                this.showToast(data.error || 'Failed to add AI player', 'error');
            }
        } catch (error) {
            console.error('Failed to add AI player:', error);
            this.showToast('Failed to add AI player', 'error');
        }
    }

    async quickPopulate() {
        if (!this.roomId) return;

        const configs = [
            { team: 'red', role: 'spymaster', search: 'anthropic/claude-sonnet-4.5' },
            { team: 'red', role: 'operative', search: 'google/gemini-3-flash-preview' },
            { team: 'blue', role: 'spymaster', search: 'x-ai/grok-4.1-fast' }
        ];

        for (const config of configs) {
            // Find best matching model
            let modelId = 'openai/gpt-4o-mini'; // fallback
            if (this.availableModels && this.availableModels.length > 0) {
                const match = this.availableModels.find(m => 
                    m.id.toLowerCase().includes(config.search) || 
                    m.name.toLowerCase().includes(config.search)
                );
                if (match) {
                    modelId = match.id;
                }
            }

            try {
                await fetch(`/api/rooms/${this.roomId}/ai-player`, {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/json' },
                    body: JSON.stringify({
                        name: `${config.search.split('/')[1] || config.search} Bot`,
                        team: config.team,
                        role: config.role,
                        model: modelId,
                        api_key: this.apiKey || null
                    })
                });
                // Small delay to ensure order and avoid rate limits
                await new Promise(r => setTimeout(r, 200));
            } catch (error) {
                console.error(`Failed to add ${config.search} bot:`, error);
            }
        }
        
        // Set current player as Blue Operative
        this.joinTeam('blue');
        setTimeout(() => this.setRole('operative'), 100);

        this.showToast('Quick populate complete', 'success');
        this.send({ type: 'request_state' });
    }

    // ===== WebSocket Connection =====
    connectWebSocket() {
        const protocol = window.location.protocol === 'https:' ? 'wss:' : 'ws:';
        const wsUrl = `${protocol}//${window.location.host}/ws`;

        this.ws = new WebSocket(wsUrl);

        this.ws.onopen = () => {
            console.log('WebSocket connected');
            // If we have a room ID, join it
            if (this.roomId && this.playerName) {
                this.send({
                    type: 'join_room',
                    room_id: this.roomId,
                    player_name: this.playerName,
                    player_id: this.playerId
                });
            }
        };

        this.ws.onmessage = (event) => {
            const message = JSON.parse(event.data);
            this.handleServerMessage(message);
        };

        this.ws.onclose = () => {
            console.log('WebSocket disconnected');
            this.showToast('Disconnected from server', 'error');
        };

        this.ws.onerror = (error) => {
            console.error('WebSocket error:', error);
            this.showToast('Connection error', 'error');
        };
    }

    send(message) {
        if (this.ws && this.ws.readyState === WebSocket.OPEN) {
            this.ws.send(JSON.stringify(message));
        }
    }

    // ===== Message Handling =====
    handleServerMessage(message) {
        console.log('Received:', message);

        switch (message.type) {
            case 'connected':
                if (!this.playerId) {
                    this.playerId = message.player_id;
                }
                break;

            case 'room_joined':
                this.roomId = message.room_id;
                this.playerId = message.player_id;
                this.showScreen('lobby-screen');
                break;

            case 'game_state':
                this.gameState = message.room;
                this.updateUI();
                break;

            case 'player_joined':
                this.showToast(`${message.player.name} joined`, 'info');
                this.send({ type: 'request_state' });
                break;

            case 'player_left':
                this.showToast('A player left', 'info');
                this.send({ type: 'request_state' });
                break;

            case 'player_updated':
                this.send({ type: 'request_state' });
                break;

            case 'game_started':
                this.showScreen('game-screen');
                this.showToast('Game started!', 'success');
                this.send({ type: 'request_state' });
                break;

            case 'clue_given':
                this.isAiProcessing = false;
                this.showToast(`${message.team} Spymaster: "${message.word}" ${message.number}`, 'info');
                this.send({ type: 'request_state' });
                setTimeout(() => this.continueAutoPlayIfNeeded(), 100);
                break;

            case 'guess_made':
                this.isAiProcessing = false;
                const resultText = message.result.correct ? 'Correct!' : `Wrong - ${message.result.color}`;
                this.showToast(`${message.team} guessed ${message.result.word}: ${resultText}`,
                    message.result.correct ? 'success' : 'error');
                this.send({ type: 'request_state' });
                setTimeout(() => this.continueAutoPlayIfNeeded(), 100);
                break;

            case 'turn_passed':
                this.isAiProcessing = false;
                this.showToast(`${message.team} ended their turn`, 'info');
                this.send({ type: 'request_state' });
                setTimeout(() => this.continueAutoPlayIfNeeded(), 100);
                break;

            case 'game_ended':
                this.isAiProcessing = false;
                this.showGameOver(message.winner);
                this.stopAutoPlay();
                break;

            case 'chat_message':
                this.addChatMessage(message);
                break;

            case 'ai_thinking':
                this.showToast(`AI is ${message.action}...`, 'info');
                const btn = document.getElementById('trigger-ai-btn');
                if (btn) {
                    btn.textContent = 'Thinking...';
                    this.isAiProcessing = true;
                    btn.disabled = true;
                }
                break;

            case 'game_reset':
                this.showScreen('lobby-screen');
                this.showToast('Game reset', 'info');
                this.send({ type: 'request_state' });
                break;

            case 'error':
                this.isAiProcessing = false;
                this.showToast(message.message, 'error');
                this.stopAutoPlay();
                break;
        }
    }

    // ===== UI Rendering =====
    renderRoomList(rooms) {
        const container = document.getElementById('room-list');

        if (rooms.length === 0) {
            container.innerHTML = '<p class="empty-slot">No rooms available</p>';
            return;
        }

        container.innerHTML = rooms.map(room => `
            <div class="room-item ${this.selectedRoom === room.id ? 'selected' : ''}"
                 data-room-id="${room.id}"
                 onclick="game.selectRoom('${room.id}')">
                <span class="room-item-name">${this.escapeHtml(room.name)}</span>
                <span class="room-item-info">${room.player_count} players - ${room.phase}</span>
            </div>
        `).join('');
    }

    filterModels(query) {
        if (!query) {
            this.renderModelSelect(this.availableModels);
            return;
        }
        
        const lowerQuery = query.toLowerCase();
        const filtered = this.availableModels.filter(m => 
            m.name.toLowerCase().includes(lowerQuery) || 
            m.provider.toLowerCase().includes(lowerQuery) ||
            m.id.toLowerCase().includes(lowerQuery)
        );
        this.renderModelSelect(filtered);
    }

    renderModelSelect(models) {
        const select = document.getElementById('ai-model');
        select.innerHTML = models.map(model =>
            `<option value="${model.id}">${model.name} (${model.provider})</option>`
        ).join('');
    }

    selectRoom(roomId) {
        this.selectedRoom = roomId;
        document.querySelectorAll('.room-item').forEach(item => {
            item.classList.toggle('selected', item.dataset.roomId === roomId);
        });

        // Join the room
        const playerName = document.getElementById('player-name-join').value.trim();
        if (!playerName) {
            this.showToast('Please enter your name first', 'error');
            return;
        }

        this.playerName = playerName;
        this.roomId = roomId;
        this.connectWebSocket();
    }

    updateUI() {
        if (!this.gameState) return;

        const game = this.gameState.game;
        const players = this.gameState.players;
        const currentPlayer = players.find(p => p.id === this.playerId);

        // Update room info
        document.getElementById('lobby-room-name').textContent = this.gameState.game_name;
        document.getElementById('lobby-room-id').textContent = this.gameState.room_id.substring(0, 8);
        document.getElementById('game-room-name').textContent = this.gameState.game_name;

        // Update scores
        document.getElementById('red-remaining').textContent = game.red_remaining;
        document.getElementById('blue-remaining').textContent = game.blue_remaining;

        // Update turn indicator
        const turnIndicator = document.getElementById('turn-indicator');
        const turnPhase = game.turn_phase === 'giving_clue' ? 'Giving Clue' : 'Guessing';
        turnIndicator.textContent = `${game.current_team.toUpperCase()} - ${turnPhase}`;
        turnIndicator.className = `turn-indicator ${game.current_team}`;

        // Determine if current player is spymaster
        const isSpymaster = currentPlayer && currentPlayer.role === 'spymaster';
        const isMyTeamTurn = currentPlayer && currentPlayer.team === game.current_team;
        const isMyRole = isMyTeamTurn && (
            (game.turn_phase === 'giving_clue' && isSpymaster) ||
            (game.turn_phase === 'guessing' && !isSpymaster)
        );

        if (game.phase === 'lobby') {
            this.renderLobbyPlayers(players, currentPlayer);
            this.updateStartButton(players);
        } else if (game.phase === 'playing') {
            this.renderGameBoard(game, isSpymaster, isMyRole && !isSpymaster);
            this.renderGamePlayers(players, game.current_team);
            this.renderClueHistory(game.clue_history);
            this.renderTranscript(game.transcript);
            this.updateActionPanel(game, currentPlayer, isSpymaster, isMyTeamTurn);
        }
    }

    renderLobbyPlayers(players, currentPlayer) {
        // Red team
        const redSpymaster = players.find(p => p.team === 'red' && p.role === 'spymaster');
        const redOperatives = players.filter(p => p.team === 'red' && p.role === 'operative');

        document.getElementById('red-spymaster').innerHTML = redSpymaster
            ? this.renderPlayerChip(redSpymaster)
            : '<span class="empty-slot">Empty</span>';

        document.getElementById('red-operatives').innerHTML = redOperatives.length > 0
            ? redOperatives.map(p => this.renderPlayerChip(p)).join('')
            : '<span class="empty-slot">No operatives</span>';

        document.getElementById('red-count').textContent =
            players.filter(p => p.team === 'red').length;

        // Blue team
        const blueSpymaster = players.find(p => p.team === 'blue' && p.role === 'spymaster');
        const blueOperatives = players.filter(p => p.team === 'blue' && p.role === 'operative');

        document.getElementById('blue-spymaster').innerHTML = blueSpymaster
            ? this.renderPlayerChip(blueSpymaster)
            : '<span class="empty-slot">Empty</span>';

        document.getElementById('blue-operatives').innerHTML = blueOperatives.length > 0
            ? blueOperatives.map(p => this.renderPlayerChip(p)).join('')
            : '<span class="empty-slot">No operatives</span>';

        document.getElementById('blue-count').textContent =
            players.filter(p => p.team === 'blue').length;

        // Unassigned
        const unassigned = players.filter(p => !p.team);
        document.getElementById('unassigned-players').innerHTML = unassigned.length > 0
            ? unassigned.map(p => this.renderPlayerChip(p)).join('')
            : '<span class="empty-slot">No unassigned players</span>';
    }

    renderPlayerChip(player) {
        const isMe = player.id === this.playerId;
        const isHost = this.gameState.host_id === this.playerId;
        const canRemove = isHost && !isMe;

        return `
            <span class="player-chip ${player.is_ai ? 'is-ai' : ''} ${isMe ? 'is-me' : ''}">
                ${this.escapeHtml(player.name)}
                ${player.is_ai ? ' <span class="ai-badge">AI</span>' : ''}
                ${isMe ? ' (You)' : ''}
                ${canRemove ? `<button class="remove-btn" onclick="game.removePlayer('${player.id}')">&times;</button>` : ''}
            </span>
        `;
    }

    renderGameBoard(game, isSpymaster, canGuess) {
        const board = document.getElementById('game-board');

        board.innerHTML = game.cards.map(card => {
            const classes = ['card-tile'];

            if (card.revealed) {
                classes.push('revealed');
                classes.push(card.color.toLowerCase());
            } else if (isSpymaster && card.color) {
                classes.push('spymaster-view');
                classes.push(card.color.toLowerCase());
            }

            if (canGuess && !card.revealed) {
                classes.push('selectable');
            }

            if (!canGuess && !card.revealed) {
                classes.push('disabled');
            }

            return `
                <div class="${classes.join(' ')}"
                     data-position="${card.position}"
                     onclick="game.makeGuess(${card.position})">
                    ${this.escapeHtml(card.word)}
                </div>
            `;
        }).join('');
    }

    renderGamePlayers(players, currentTeam) {
        const redPlayers = players.filter(p => p.team === 'red');
        const bluePlayers = players.filter(p => p.team === 'blue');

        document.getElementById('game-red-players').innerHTML =
            redPlayers.map(p => this.renderRosterPlayer(p, currentTeam === 'red')).join('');

        document.getElementById('game-blue-players').innerHTML =
            bluePlayers.map(p => this.renderRosterPlayer(p, currentTeam === 'blue')).join('');
    }

    renderRosterPlayer(player, isCurrentTeam) {
        const isMe = player.id === this.playerId;
        return `
            <div class="roster-player ${isCurrentTeam ? 'current-turn' : ''}">
                <span>${this.escapeHtml(player.name)}${isMe ? ' (You)' : ''}</span>
                <span class="role-badge">${player.role || 'No Role'}</span>
                ${player.is_ai ? '<span class="ai-badge">AI</span>' : ''}
            </div>
        `;
    }

    renderClueHistory(history) {
        const container = document.getElementById('clue-history');

        if (history.length === 0) {
            container.innerHTML = '<p class="empty-slot">No clues yet</p>';
            return;
        }

        container.innerHTML = history.map(clue => `
            <div class="clue-entry ${clue.team}">
                <span class="clue-entry-word">${this.escapeHtml(clue.word)}</span>
                <span class="clue-entry-number">${clue.number}</span>
            </div>
        `).reverse().join('');
    }

    renderTranscript(transcript) {
        const container = document.getElementById('game-transcript');
        if (!transcript) return;

        if (transcript.length === 0) {
            container.innerHTML = '<p class="empty-slot">No transcript yet</p>';
            return;
        }

        container.innerHTML = transcript.map(log => {
            // Simple markdown parser for bold text
            const formatted = this.escapeHtml(log)
                .replace(/\*\*(.*?)\*\*/g, '<strong>$1</strong>');
            
            return `<div class="transcript-entry">${formatted}</div>`;
        }).join('');
        
        container.scrollTop = container.scrollHeight;
    }

    updateActionPanel(game, currentPlayer, isSpymaster, isMyTeamTurn) {
        // 1. Reset all displays
        document.getElementById('clue-input-panel').style.display = 'none';
        document.getElementById('current-clue-panel').style.display = 'none';
        document.getElementById('ai-action-panel').style.display = 'none';
        document.getElementById('autoplay-panel').style.display = 'none';
        document.getElementById('waiting-panel').style.display = 'none';
        const passBtn = document.getElementById('pass-turn-btn');
        passBtn.style.display = 'none';

        if (!currentPlayer) return;

        // 2. Global Info: Current Clue (Visible to everyone in Guessing phase)
        if (game.turn_phase === 'guessing' && game.current_clue) {
            document.getElementById('current-clue-panel').style.display = 'block';
            document.getElementById('current-clue-word').textContent = game.current_clue.word;
            document.getElementById('current-clue-number').textContent = game.current_clue.number;
            document.getElementById('guesses-made').textContent = game.current_clue.guesses_made;
            document.getElementById('guesses-allowed').textContent = game.current_clue.number + 1;
            
            // Show Pass Button ONLY if it's my turn and I'm an operative
            if (isMyTeamTurn && !isSpymaster) {
                passBtn.style.display = 'block';
            }
        }

        // 3. AI Controls
        const currentTeamPlayers = this.gameState.players.filter(p => p.team === game.current_team);
        const activeAI = currentTeamPlayers.find(p => {
            if (!p.is_ai) return false;
            if (game.turn_phase === 'giving_clue') return p.role === 'spymaster';
            return p.role === 'operative';
        });

        const isHost = this.gameState.host_id === this.playerId;
        if (activeAI && (isMyTeamTurn || isHost)) {
            document.getElementById('ai-action-panel').style.display = 'block';
            document.getElementById('ai-thinking-text').textContent =
                `${activeAI.name} is ready to ${game.turn_phase === 'giving_clue' ? 'give a clue' : 'guess'}`;
            const btn = document.getElementById('trigger-ai-btn');
            btn.dataset.aiId = activeAI.id;
            
            if (this.isAiProcessing) {
                btn.disabled = true;
                btn.textContent = 'Processing...';
            } else {
                btn.disabled = false;
                btn.textContent = 'Trigger AI Action';
            }
        }

        // 4. Auto-play Panel - Show if there are any AI players in the game
        const hasAI = this.gameState.players.some(p => p.is_ai);
        if (hasAI && game.phase !== 'finished') {
            document.getElementById('autoplay-panel').style.display = 'block';
            this.updateAutoPlayUI();
        }

        // 5. Human Spymaster Input
        if (isMyTeamTurn && game.turn_phase === 'giving_clue' && isSpymaster) {
            document.getElementById('clue-input-panel').style.display = 'block';
        }

        // 6. Waiting State
        let showWaiting = !isMyTeamTurn;
        
        if (isMyTeamTurn) {
            if (game.turn_phase === 'giving_clue' && !isSpymaster) showWaiting = true; // Operative waiting for Spymaster
            if (game.turn_phase === 'guessing' && isSpymaster) showWaiting = true; // Spymaster waiting for Operative
        }

        // If AI controls are visible, we don't need waiting text
        if (document.getElementById('ai-action-panel').style.display === 'block') {
            showWaiting = false;
        }

        if (showWaiting) {
            document.getElementById('waiting-panel').style.display = 'block';
            const waitingText = document.getElementById('waiting-text');
            
            if (!isMyTeamTurn) {
                waitingText.textContent = `Waiting for ${game.current_team.toUpperCase()} team...`;
            } else if (game.turn_phase === 'giving_clue') {
                waitingText.textContent = 'Waiting for Spymaster clue...';
            } else {
                waitingText.textContent = 'Waiting for Operatives to guess...';
            }
        }
    }

    updateStartButton(players) {
        const redCount = players.filter(p => p.team === 'red').length;
        const blueCount = players.filter(p => p.team === 'blue').length;
        const redSpymaster = players.some(p => p.team === 'red' && p.role === 'spymaster');
        const blueSpymaster = players.some(p => p.team === 'blue' && p.role === 'spymaster');

        const canStart = redCount >= 1 && blueCount >= 1 && redSpymaster && blueSpymaster;
        const isHost = this.gameState.host_id === this.playerId;

        document.getElementById('start-game-btn').disabled = !canStart || !isHost;
    }

    // ===== Game Actions =====
    joinTeam(team, role) {
        this.send({ type: 'set_team', team });
        if (role) {
            this.send({ type: 'set_role', role });
        }
    }

    setRole(role) {
        this.send({ type: 'set_role', role });
    }

    startGame() {
        this.send({ type: 'start_game' });
    }

    giveClue() {
        const word = document.getElementById('clue-word').value.trim();
        const number = parseInt(document.getElementById('clue-number').value) || 1;

        if (!word) {
            this.showToast('Please enter a clue word', 'error');
            return;
        }

        if (word.includes(' ')) {
            this.showToast('Clue must be a single word', 'error');
            return;
        }

        this.send({ type: 'give_clue', word, number });
        document.getElementById('clue-word').value = '';
        document.getElementById('clue-number').value = '1';
    }

    makeGuess(position) {
        const currentPlayer = this.gameState?.players.find(p => p.id === this.playerId);
        const game = this.gameState?.game;

        if (!currentPlayer || !game) return;

        // Check if player can guess
        const isMyTeamTurn = currentPlayer.team === game.current_team;
        const isOperative = currentPlayer.role === 'operative';
        const isGuessing = game.turn_phase === 'guessing';

        if (!isMyTeamTurn || !isOperative || !isGuessing) return;

        // Check if card is already revealed
        const card = game.cards.find(c => c.position === position);
        if (card && card.revealed) return;

        this.send({ type: 'make_guess', position });
    }

    passTurn() {
        this.send({ type: 'pass_turn' });
    }

    triggerAiAction() {
        const btn = document.getElementById('trigger-ai-btn');
        const aiId = btn.dataset.aiId;
        if (aiId) {
            this.isAiProcessing = true;
            btn.disabled = true;
            btn.textContent = 'Processing...';
            this.send({ type: 'request_ai_action', player_id: aiId });
        }
    }

    startAutoPlay() {
        this.isAutoPlaying = true;
        this.updateAutoPlayUI();
        this.continueAutoPlayIfNeeded();
    }

    stopAutoPlay() {
        this.isAutoPlaying = false;
        this.updateAutoPlayUI();
    }

    updateAutoPlayUI() {
        const autoplayBtn = document.getElementById('autoplay-btn');
        const stopBtn = document.getElementById('stop-autoplay-btn');
        const autoplayText = document.getElementById('autoplay-text');

        if (this.isAutoPlaying) {
            autoplayBtn.style.display = 'none';
            stopBtn.style.display = 'block';
            autoplayText.textContent = 'Auto-play is running...';
        } else {
            autoplayBtn.style.display = 'block';
            stopBtn.style.display = 'none';
            autoplayText.textContent = 'Run all AI turns automatically';
        }
    }

    continueAutoPlayIfNeeded() {
        // Don't continue if auto-play is disabled or we're already processing
        if (!this.isAutoPlaying || this.isAiProcessing) {
            return;
        }

        // Don't continue if game is over or no game state
        if (!this.gameState || !this.gameState.game || this.gameState.game.phase === 'finished') {
            if (this.gameState && this.gameState.game && this.gameState.game.phase === 'finished') {
                this.stopAutoPlay();
            }
            return;
        }

        const game = this.gameState.game;

        // Check if current active player is AI
        const currentTeamPlayers = this.gameState.players.filter(p => p.team === game.current_team);
        const activeAI = currentTeamPlayers.find(p => {
            if (!p.is_ai) return false;
            if (game.turn_phase === 'giving_clue') return p.role === 'spymaster';
            return p.role === 'operative';
        });

        // If there's an active AI, trigger it after a short delay for visibility
        if (activeAI) {
            setTimeout(() => {
                // Double-check we're still auto-playing
                if (this.isAutoPlaying && !this.isAiProcessing) {
                    const btn = document.getElementById('trigger-ai-btn');
                    if (btn && btn.dataset.aiId) {
                        this.triggerAiAction();
                    }
                }
            }, 500); // 500ms delay so users can see what's happening
        } else {
            // No AI to play, stop auto-play
            this.stopAutoPlay();
        }
    }

    resetGame() {
        document.getElementById('game-over-modal').style.display = 'none';
        this.send({ type: 'reset_game' });
    }

    sendChat() {
        const input = document.getElementById('chat-input');
        const message = input.value.trim();

        if (!message) return;

        this.send({ type: 'chat', message });
        input.value = '';
    }

    addChatMessage(data) {
        const container = document.getElementById('chat-messages');
        const messageEl = document.createElement('div');
        messageEl.className = 'chat-message';
        messageEl.innerHTML = `
            <span class="chat-message-sender">${this.escapeHtml(data.player_name)}</span>
            <span class="chat-message-time">${data.timestamp}</span>
            <div>${this.escapeHtml(data.message)}</div>
        `;
        container.appendChild(messageEl);
        container.scrollTop = container.scrollHeight;
    }

    async removePlayer(playerId) {
        try {
            await fetch(`/api/rooms/${this.roomId}/players/${playerId}`, {
                method: 'DELETE'
            });
        } catch (error) {
            this.showToast('Failed to remove player', 'error');
        }
    }

    // ===== Navigation =====
    showScreen(screenId) {
        document.querySelectorAll('.screen').forEach(screen => {
            screen.classList.remove('active');
        });
        document.getElementById(screenId).classList.add('active');
    }

    leaveLobby() {
        this.send({ type: 'leave_room' });
        if (this.ws) {
            this.ws.close();
            this.ws = null;
        }
        this.roomId = null;
        this.gameState = null;
        this.showScreen('landing-screen');
        this.fetchRooms();
    }

    backToLobby() {
        document.getElementById('game-over-modal').style.display = 'none';
        this.leaveLobby();
    }

    showGameOver(winner) {
        const modal = document.getElementById('game-over-modal');
        const content = modal.querySelector('.modal-content');
        const winnerText = document.getElementById('winner-text');

        content.className = `modal-content game-over-content ${winner}-winner`;
        winnerText.textContent = `${winner.toUpperCase()} TEAM WINS!`;

        modal.style.display = 'flex';
    }

    // ===== Utilities =====
    showToast(message, type = 'info') {
        const container = document.getElementById('toast-container');
        const toast = document.createElement('div');
        toast.className = `toast ${type}`;
        toast.textContent = message;

        container.appendChild(toast);

        setTimeout(() => {
            toast.style.opacity = '0';
            setTimeout(() => toast.remove(), 300);
        }, 3000);
    }

    escapeHtml(text) {
        const div = document.createElement('div');
        div.textContent = text;
        return div.innerHTML;
    }
}

// Initialize game when DOM is ready
let game;
document.addEventListener('DOMContentLoaded', () => {
    game = new CodenamesGame();
});
